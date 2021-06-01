/* 
======================================================
		ASTROMECH DROID BRAIN
====================================================== 
   This started out as a Hello World Example
   but has morphed into something much different...

legal stuff i dont quite understand:

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


//ESP-Cam driver
#include "esp_camera.h"

//WiFi soft AP libraries
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_netif.h"

//server libraries
//#include "protocol_examples_common.h"
#include <esp_http_server.h>

//SPIFFS and SD File support
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/param.h>
#include "esp_spiffs.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
//#include "nvs_flash.h"
#include "sdkconfig.h"
//#include "driver/sdspi_host.h"
//#include "driver/spi_common.h"
//#include "sdmmc_cmd.h"
//#include "soc/soc_caps.h"
//#if SOC_SDMMC_HOST_SUPPORTED
//#include "driver/sdmmc_host.h"
//#endif

//I2C LCD Libraries
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp32/rom/uart.h"
#include "smbus.h"
#include "i2c-lcd1602.h"
// # include " cJSON.h"

#ifdef USE_SPI_MODE
#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   13
#endif

#define SPI_DMA_CHAN    1
#define SPI_DMA_CHAN    1

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/

// ESP32Cam (AiThinker) PIN Map
//#ifdef BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

//#endif

// LCD1602
#define LCD_NUM_ROWS               2
#define LCD_NUM_COLUMNS            32
#define LCD_NUM_VISIBLE_COLUMNS    16

#define I2C_MASTER_NUM           I2C_NUM_0
#define I2C_MASTER_TX_BUF_LEN    0                     // disabled
#define I2C_MASTER_RX_BUF_LEN    0                     // disabled
#define I2C_MASTER_FREQ_HZ       100000
#define I2C_MASTER_SDA_IO        14  //CONFIG_I2C_MASTER_SDA
#define I2C_MASTER_SCL_IO        13  //CONFIG_I2C_MASTER_SCL
#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/

#define CONFIG_LCD1602_I2C_ADDRESS 0x27  //0x3F
#define ARDUINO_I2C_ADDRESS 0x5F //change to match Arduino Sketch

esp_netif_t *ap_netif = NULL;
//WiFi SSID
#define MY_ESP_WIFI_SSID      "roboFi"//CONFIG_ESP_WIFI_SSID
#define MY_ESP_WIFI_PASS     ""// CONFIG_ESP_WIFI_PASSWORD
#define MY_ESP_WIFI_CHANNEL  1 //CONFIG_ESP_WIFI_CHANNEL
#define MY_MAX_STA_CONN     3 // CONFIG_ESP_MAX_STA_CONN

static const char* TAG = "astromechDroid_BRAIN";

static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sscb_sda = CAM_PIN_SIOD,
    .pin_sscb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_VGA,    //QQVGA-UXGA Do not use sizes above QVGA when not JPEG

    .jpeg_quality = 12, //0-63 lower number means higher quality
    .fb_count = 1       //if more than one, i2s runs in continuous mode. Use only with JPEG
};

static esp_err_t init_camera()
{
    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        printf( "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}

//=========================================================================
static void i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf={
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .sda_pullup_en = GPIO_PULLUP_DISABLE,  // GY-2561 provides 10kΩ pullups
    .scl_io_num = I2C_MASTER_SCL_IO,
    .scl_pullup_en = GPIO_PULLUP_DISABLE,  // GY-2561 provides 10kΩ pullups
    .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_LEN,
                       I2C_MASTER_TX_BUF_LEN, 0);
}




void sendArduinoI2C_cmd( uint8_t *data_wr, size_t size)
{
//  i2c_master_init();
  i2c_port_t i2c_num = I2C_MASTER_NUM;
  uint8_t arduinoAddress = ARDUINO_I2C_ADDRESS;
	//I2C Write..

	ESP_LOGE(TAG,"Starting I2C to Arduino..");
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd,(arduinoAddress<<1) | WRITE_BIT,ACK_CHECK_EN );
	i2c_master_write(cmd,data_wr,size,ACK_CHECK_EN);
	i2c_master_stop(cmd);

   esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	if(ret != ESP_OK)	
		ESP_LOGE(TAG,"SENDING TO ARDUINO FAILED X(");
 i2c_cmd_link_delete(cmd); 
//vTaskDelete(NULL);

}




void lcdPrintIP( void *pvParam)
{


i2c_master_init();
i2c_port_t i2c_num = I2C_MASTER_NUM;


uint8_t address = CONFIG_LCD1602_I2C_ADDRESS;

    // Set up the SMBus
    smbus_info_t * smbus_info = smbus_malloc();
    ESP_ERROR_CHECK(smbus_init(smbus_info, i2c_num, address));
    ESP_ERROR_CHECK(smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS));

    // Set up the LCD1602 device with backlight off
    i2c_lcd1602_info_t * lcd_info = i2c_lcd1602_malloc();
    ESP_ERROR_CHECK(i2c_lcd1602_init(lcd_info, smbus_info, true,
                                     LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VISIBLE_COLUMNS));

    ESP_ERROR_CHECK(i2c_lcd1602_reset(lcd_info));


uint8_t custom1[8] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x1F,
  0x00,
  0x00,
  0x00
};
uint8_t customDither[8] = {
  0x12,
  0x09,
  0x04,
  0x12,
  0x09,
  0x04,
  0x12,
  0x09
};


uint8_t custom8[8]= {
  0x00,
  0x00,
  0x00,
  0x1F,
  0x15,
  0x1F,
  0x00,
  0x00
};
uint8_t custom4[8]= {
    0x00,
  0x00,
  0x00,
  0x1F,
  0x04,
  0x1C,
  0x00,
  0x00
};
uint8_t custom9[8] = {
   0x00,
  0x00,
  0x00,
  0x1F,
  0x14,
  0x1C,
  0x00,
  0x00
  
  };

uint8_t customLambda[8]    = {
  0x00,
  0x02,
  0x01,
  0x03,
  0x04,
  0x1F,
  0x10,
  0x00
};


uint8_t custom2[8]={
   0x00,
  0x00,
  0x00,
  0x1D,
  0x15,
  0x17,
  0x00,
  0x00
};

uint8_t custom6[8]={
  0x00,
  0x00,
  0x00,
  0x1F,
  0x15,
  0x17,
  0x00,
  0x00

};

   i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_0, custom1);
   i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1, custom4);
   i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2, custom8);
   i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_3, custom9);
   i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_4, customDither);
   i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_5, customLambda);
   i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_6, custom2);
   i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_7, custom6);

// lcd print:
// custom1, dither
// custom9 Z
// . custom1
// b custom8
// . custom4
// . custom1
//
   i2c_lcd1602_move_cursor(lcd_info, 1, 1);
   i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_0 );//1

   i2c_lcd1602_move_cursor(lcd_info, 0, 0);
   i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_4 );//dither
   i2c_lcd1602_move_cursor(lcd_info, 0, 1);
   i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_4 );//dither

   i2c_lcd1602_move_cursor(lcd_info, 2, 1);
   i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_3 );//9
  
   i2c_lcd1602_move_cursor(lcd_info, 3,1 );
   i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_6 );//2

    i2c_lcd1602_move_cursor(lcd_info, 4, 1);
    i2c_lcd1602_write_char(lcd_info, '.' );

    i2c_lcd1602_move_cursor(lcd_info, 5, 1);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_0 );//1

    i2c_lcd1602_move_cursor(lcd_info, 6, 1);
    i2c_lcd1602_write_char(lcd_info,   I2C_LCD1602_INDEX_CUSTOM_7 );//6

    i2c_lcd1602_move_cursor(lcd_info, 7, 1);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2 );//8
    
    i2c_lcd1602_move_cursor(lcd_info, 8, 1);
    i2c_lcd1602_write_char(lcd_info, '.' );

    i2c_lcd1602_move_cursor(lcd_info, 9, 1);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1 );//4

    i2c_lcd1602_move_cursor(lcd_info, 10, 1);
    i2c_lcd1602_write_char(lcd_info, '.' );

    i2c_lcd1602_move_cursor(lcd_info, 11, 1);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_0 );//1


    i2c_lcd1602_move_cursor(lcd_info, 12, 0);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_5 );//lambda
/*
    i2c_lcd1602_move_cursor(lcd_info, 0, 0);
    i2c_lcd1602_write_char(lcd_info, custom1 );
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_INFINITY);
*/
    vTaskDelete(NULL);
}
//
//=========================================================================
//			WIFI INIT 

static void wifi_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}



void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = MY_ESP_WIFI_SSID,
            .ssid_len = strlen(MY_ESP_WIFI_SSID),
            .channel = MY_ESP_WIFI_CHANNEL,
            .password = MY_ESP_WIFI_PASS,
            .max_connection = MY_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(MY_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             MY_ESP_WIFI_SSID, MY_ESP_WIFI_PASS, MY_ESP_WIFI_CHANNEL);
}



//==========================
//	URI HANDLERS






struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
};






/* Our URI handler function to be called during GET /uri request */
esp_err_t serveIndex(httpd_req_t *req)
{

	char filepath[1024];
	int fd = open("/server/index.html",O_RDONLY,0);
	char scratch [1024];

	if(fd ==-1)
	{
	 ESP_LOGE(TAG,"FAILED TO OPEN INDEX.HTML");
 	 httpd_resp_send_err(req,HTTPD_500_INTERNAL_SERVER_ERROR,"Failed to read existing file.");
 		return ESP_FAIL;
	}

httpd_resp_set_type(req,"text/html");
char *chunk = scratch;
ssize_t read_bytes;
	do{
		read_bytes=read(fd,chunk,1024);
		if(read_bytes==-1)
			{ 
			  ESP_LOGE(TAG,"FAILED TO READ FILE BYTES!");
			}
		else if (read_bytes > 0 )
			{
			  if(httpd_resp_send_chunk
			(req, chunk, read_bytes) != ESP_OK) {
                	close(fd);
                	ESP_LOGE(TAG, "File sending failed!");
        	        /* Abort sending file */
                	httpd_resp_sendstr_chunk(req, NULL);
               		/* Respond with 500 Internal Server Error */
                	httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
close(fd);
    return ESP_OK;
}



//=======================================================================
//ASYNC WEBSOCKET
//change to I2C comm to Arduino

/*
 * async send function, which we put into the httpd work queue
 */
static void ws_async_send(void *arg)
{
    static const char * data = "Async data";
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t*)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
//=============================================================
//sendArduinoI2C_cmd(ws_pkt.payload,ws_pkt.len);
//=============================================================
    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req)
{
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    return httpd_queue_work(handle, ws_async_send, resp_arg);
}



static esp_err_t webSocket_echo(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len) {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
        //=============================================================
	sendArduinoI2C_cmd(ws_pkt.payload,ws_pkt.len);
	//=============================================================
    }
    ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);
    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT &&
        strcmp((char*)ws_pkt.payload,"Trigger async") == 0) {
        free(buf);
        return trigger_async_send(req->handle, req);
    }

    ret = httpd_ws_send_frame(req, &ws_pkt);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
    }
    free(buf);
    return ret;
}





//static const httpd_uri_t index

typedef struct {
        httpd_req_t *req;
        size_t len;
} jpg_chunking_t;

static size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len){
    jpg_chunking_t *j = (jpg_chunking_t *)arg;
    if(!index){
        j->len = 0;
    }
    if(httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK){
        return 0;
    }
    j->len += len;
    return len;
}


esp_err_t jpg_httpd_handler(httpd_req_t *req){
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t fb_len = 0;
    int64_t fr_start = esp_timer_get_time();

    fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    res = httpd_resp_set_type(req, "image/jpeg");
    if(res == ESP_OK){
        res = httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    }

    if(res == ESP_OK){
        if(fb->format == PIXFORMAT_JPEG){
            fb_len = fb->len;
            res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
        } else {
            jpg_chunking_t jchunk = {req, 0};
            res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk)?ESP_OK:ESP_FAIL;
            httpd_resp_send_chunk(req, NULL, 0);
            fb_len = jchunk.len;
        }
    }
    esp_camera_fb_return(fb);
    int64_t fr_end = esp_timer_get_time();
    ESP_LOGI(TAG, "JPG: %uKB %ums", (uint32_t)(fb_len/1024), (uint32_t)((fr_end - fr_start)/1000));
    return res;
}


//JPEG HANDLER
static const httpd_uri_t camStream=
{
   .uri= "/camStream",
   .method = HTTP_GET,
   .handler = jpg_httpd_handler,
   .user_ctx=NULL
};

//INDEX.html
static const httpd_uri_t makeIndex =
{ 
	.uri="/",
	.method=HTTP_GET,
	.handler=serveIndex,
	.user_ctx=NULL

};
// ROBOT CONTROL URI
//HTTP_POST (i get this from user)


static const httpd_uri_t roboControl=
{
  .uri="/rctrl",
  .method = HTTP_GET,
  .handler=webSocket_echo,// send I2C on data
  .user_ctx=NULL,
  .is_websocket=true
};


//========================================
//	START SERVER / URI REGISTRATION
static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);


    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &camStream);
        httpd_register_uri_handler(server, &makeIndex);
        httpd_register_uri_handler(server, &roboControl);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}


//=========================================================================
//			SPIFFS FILESYSTEM AND SD CARD
/* Function to initialize SPIFFS */
static esp_err_t init_spiffs(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/server",
      .partition_label = NULL,
      .max_files = 5,   // This decides the maximum number of files that can be created on the storage
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

//==============================================================================



void app_main(void){
	
xTaskCreate(&lcdPrintIP,"lcdPrintIP",4096,NULL,5,NULL);	
static httpd_handle_t server = NULL;

//init hardware camera, Screen, I2C to Motor Control

init_spiffs();
init_camera();

//start the wifi stuff
// NVS
esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

//printf("starting AP");
    wifi_init_softap();




// INIT SERVER
//ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
//ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));




server = start_webserver();
//printf(server.ip);

    /*Mount the SDcard first if needed.*/
//#ifdef CONFIG_EXAMPLE_MOUNT_SD_CARD
//    sdcard_mount();
//#endif

//    ESP_ERROR_CHECK(esp_netif_init());
//    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
  //  ESP_ERROR_CHECK(example_connect());

    /* Initialize file storage */
    //ESP_ERROR_CHECK(init_spiffs());

    /* Start the file server */
    //ESP_ERROR_CHECK(start_file_server("/spiffs"));

//#ifdef CONFIG_EXAMPLE_MOUNT_SD_CARD
    //deinitialize the bus after all devices are removed
    //ESP_ERROR_CHECK(unmount_card(mount_base_path, mount_card));
//#endif

//esp_netif_ip_info_t ipInfo; 
//char str[256];
// IP address.
//esp_netif_get_ip_info(ap_netif, &ipInfo);
//printf("My IP: "IPSTR"\n", IP2STR(&ipInfo.ip));

printf("All Setup!\n");

    /* Print chip information 
    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
         printf( "Taking picture...");
        camera_fb_t *pic = esp_camera_fb_get();

        // use pic->buf to access the image
        printf("Picture taken! Its size was: %zu bytes", pic->len);

        
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
*/
    printf("waiting for something to happen.....\n");
    fflush(stdout);
// while(true)
 //{
// printf(".\n");
// }	
 
    //esp_restart();
}




