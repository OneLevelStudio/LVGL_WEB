/**
 * @file main
 */
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <emscripten.h>
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "examplelist.h"
#if defined(__APPLE__) && defined(TARGET_OS_MAC)
#if __APPLE__ && TARGET_OS_MAC
#define SDL_APPLE
#endif
#endif
static void hal_init(void);
static int tick_thread(void *data);
static void memory_monitor(lv_timer_t *param);
int monitor_hor_res, monitor_ver_res;
void do_loop(void *arg);
static void lv_example_noop(void)
{
}

#include <unistd.h>
#define HARDWARE "LVGL_WEB"
#define lv_menu_get_main_header_back_btn lv_menu_get_main_header_back_button
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
static lv_obj_t *obj_menu;
static lv_style_t style_buttonlarge;
static lv_style_t style_buttonsmall;

#define color_header_background 0x000000
#define color_header_titletext 0xffffff
#define color_header_backtext 0x808080
#define color_header_border 0xffffff
#define color_global_background 0x000000
#define color_btnlarge_background 0x000000
#define color_btnlarge_border 0xffffff
#define color_btnlarge_text 0xffffff
#define color_btnsmall_background 0x000000
#define color_btnsmall_border 0xffffff
#define color_btnsmall_text 0xffffff
#define size_borderwidth 2
#define size_borderradius 4

void init_styles(void)
{
    lv_style_init(&style_buttonlarge);
    lv_style_set_radius(&style_buttonlarge, size_borderradius);
    lv_style_set_width(&style_buttonlarge, lv_pct(100));
    lv_style_set_pad_ver(&style_buttonlarge, 15);
    lv_style_set_bg_opa(&style_buttonlarge, LV_OPA_COVER);
    lv_style_set_bg_color(&style_buttonlarge, lv_color_hex(color_btnlarge_background));
    lv_style_set_border_width(&style_buttonlarge, size_borderwidth);
    lv_style_set_border_color(&style_buttonlarge, lv_color_hex(color_btnlarge_border));
    lv_style_set_text_color(&style_buttonlarge, lv_color_hex(color_btnlarge_text));

    lv_style_init(&style_buttonsmall);
    lv_style_set_radius(&style_buttonsmall, size_borderradius);
    lv_style_set_width(&style_buttonsmall, lv_pct(100));
    lv_style_set_pad_ver(&style_buttonsmall, 10);
    lv_style_set_bg_opa(&style_buttonsmall, LV_OPA_COVER);
    lv_style_set_bg_color(&style_buttonsmall, lv_color_hex(color_btnsmall_background));
    lv_style_set_border_width(&style_buttonsmall, size_borderwidth);
    lv_style_set_border_color(&style_buttonsmall, lv_color_hex(color_btnsmall_border));
    lv_style_set_text_color(&style_buttonsmall, lv_color_hex(color_btnsmall_text));
}

// ---------- Page: Keyboard Test ----------
static lv_obj_t *obj_resulttext;
static lv_obj_t *obj_textinput;
static lv_obj_t *obj_keyboard;
static void fn_textinput_eventcb(lv_event_t *evt)
{
    lv_event_code_t evt_code = lv_event_get_code(evt);
    // Keyboard OK/SUBMIT button pressed -> obj_resulttext
    if (evt_code == LV_EVENT_READY)
    {
        const char *newtext = lv_textarea_get_text(obj_textinput);
        lv_textarea_set_text(obj_resulttext, newtext);
    }
    // obj_textinput focused -> show obj_keyboard
    if (evt_code == LV_EVENT_FOCUSED)
    {
        lv_obj_clear_flag(obj_keyboard, LV_OBJ_FLAG_HIDDEN);
    }
    // obj_textinput not focused -> hide obj_keyboard
    if (evt_code == LV_EVENT_DEFOCUSED)
    {
        lv_obj_add_flag(obj_keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

// ---------- Page: WiFi Scan ----------
static lv_obj_t *obj_wifiscan_btnscan_label;
static lv_obj_t *obj_wifiscan_listwifi_cont;
static lv_obj_t *obj_page_wifiinfo;
static lv_obj_t *obj_wifiinfo_label;
static lv_obj_t *obj_page_captiveportal;
static lv_obj_t *obj_captiveportal_label;
static String captiveportal_ap_ssid = "Free WiFi";
// static const char *get_wifi_encryption_type(wifi_auth_mode_t auth_mode)
// {
//     switch (auth_mode)
//     {
//     case WIFI_AUTH_OPEN:
//         return "OPEN";
//     case WIFI_AUTH_WEP:
//         return "WEP";
//     case WIFI_AUTH_WPA_PSK:
//         return "WPA";
//     case WIFI_AUTH_WPA2_PSK:
//         return "WPA2";
//     case WIFI_AUTH_WPA_WPA2_PSK:
//         return "WPA+WPA2";
//     case WIFI_AUTH_WPA2_ENTERPRISE:
//         return "WPA2-EAP";
//     case WIFI_AUTH_WPA3_PSK:
//         return "WPA3";
//     case WIFI_AUTH_WPA2_WPA3_PSK:
//         return "WPA2+WPA3";
//     case WIFI_AUTH_WAPI_PSK:
//         return "WAPI";
//     default:
//         return "UNK";
//     }
// }
static void fn_apdetailpage_eventcb(lv_event_t *evt)
{
    int idx = (int)(intptr_t)lv_event_get_user_data(evt);

    // // --------------------------------------------------
    // captiveportal_ap_ssid = WiFi.SSID(idx);
    // uint8_t *bssid = WiFi.BSSID(idx);
    // char strbuf_bssid[18] = "";
    // if (bssid)
    // {
    //     snprintf(strbuf_bssid, sizeof(strbuf_bssid), "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    // }
    // char strbuf[256];
    // snprintf(strbuf, sizeof(strbuf), "SSID: %s\nRSSI: %d dBm\nChannel: %d\nBSSID: %s\nEncryption Type: %s", captiveportal_ap_ssid.c_str(), WiFi.RSSI(idx), WiFi.channel(idx), strbuf_bssid, get_wifi_encryption_type(WiFi.encryptionType(idx)));
    // // --------------------------------------------------
    char strbuf[256];
    snprintf(strbuf, sizeof(strbuf), "SSID: WiFi Network #%d\nRSSI: 12345 dBm\nChannel: 123\nBSSID: 12-34-56-78-89\nEncryption Type: ABC", idx);
    // // --------------------------------------------------

    lv_label_set_text(obj_wifiinfo_label, strbuf);
    lv_menu_set_page(obj_menu, obj_page_wifiinfo);
}
static void fn_scanaps_eventcb(lv_event_t *evt)
{
    lv_event_code_t evt_code = lv_event_get_code(evt);
    // obj_btn_wifiscan clicked
    if (evt_code == LV_EVENT_CLICKED)
    {
        lv_obj_clean(obj_wifiscan_listwifi_cont);
        lv_label_set_text(obj_wifiscan_btnscan_label, "Scanning...");
        lv_refr_now(NULL);

        // // --------------------------------------------------
        // WiFi.mode(WIFI_STA);
        // WiFi.disconnect();
        // delay(100);
        // int n_aps = WiFi.scanNetworks(false, true); // (async, show_hidden)
        // if (n_aps < 0)
        // {
        //     lv_label_set_text(obj_wifiscan_btnscan_label, "Status: Failed");
        //     return;
        // }
        // for (int idx = 0; idx < n_aps; ++idx)
        // {
        //     lv_obj_t *obj_btn_item_ap = lv_btn_create(obj_wifiscan_listwifi_cont);
        //     lv_obj_add_style(obj_btn_item_ap, &style_buttonsmall, LV_PART_MAIN | LV_STATE_DEFAULT);
        //     lv_obj_t *obj_btn_item_ap_label = lv_label_create(obj_btn_item_ap);
        //     char strbuf[96];
        //     snprintf(strbuf, sizeof(strbuf), "%s [%s] (%d dBm)", WiFi.SSID(idx).c_str(), get_wifi_encryption_type(WiFi.encryptionType(idx)), WiFi.RSSI(idx));
        //     lv_label_set_text(obj_btn_item_ap_label, strbuf);
        //     lv_obj_add_event_cb(obj_btn_item_ap, fn_apdetailpage_eventcb, LV_EVENT_CLICKED, (void *)(intptr_t)idx);
        // }
        // // --------------------------------------------------
        usleep(500000); // = delay(500);
        int n_aps = 10;
        for (int idx = 0; idx < n_aps; ++idx)
        {
            lv_obj_t *obj_btn_item_ap = lv_btn_create(obj_wifiscan_listwifi_cont);
            lv_obj_add_style(obj_btn_item_ap, &style_buttonsmall, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_t *obj_btn_item_ap_label = lv_label_create(obj_btn_item_ap);
            char strbuf[96];
            snprintf(strbuf, sizeof(strbuf), "Wifi Network #%d [%s] (%d dBm)", idx, "Encryption Type", 12345);
            lv_label_set_text(obj_btn_item_ap_label, strbuf);
            lv_obj_add_event_cb(obj_btn_item_ap, fn_apdetailpage_eventcb, LV_EVENT_CLICKED, (void *)(intptr_t)idx);
        }
        // // --------------------------------------------------

        char strbuf[64];
        snprintf(strbuf, sizeof(strbuf), "Status: %d AP(s) found", n_aps);
        lv_label_set_text(obj_wifiscan_btnscan_label, strbuf);
    }
}

void handleCaptivePortal()
{
    String portal_html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body><h2>Captive Portal is working.</h2></body></html>";
    webServer.send(200, "text/html", portal_html);
}
static void fn_captiveportal_eventcb(lv_event_t *evt)
{
    lv_event_code_t evt_code = lv_event_get_code(evt);
    // obj_wifiinfo_btnattack clicked
    if (evt_code == LV_EVENT_CLICKED)
    {
        lv_menu_set_page(obj_menu, obj_page_captiveportal);

        // // --------------------------------------------------
        // WiFi.mode(WIFI_AP);
        // WiFi.disconnect();
        // delay(100);
        // WiFi.softAP(captiveportal_ap_ssid + " (Testing)");
        // dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
        // webServer.on("/", handleCaptivePortal);
        // webServer.onNotFound(handleCaptivePortal);
        // webServer.begin();
        // // --------------------------------------------------
    }
}
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================

int main(int argc, char **argv)
{
    extern const struct lv_ci_example lv_ci_example_list[];
    const struct lv_ci_example *ex = NULL;
    monitor_hor_res = 240;
    monitor_ver_res = 320;
    if (argc >= 4 && strcmp(ex->name, "default"))
    {
        for (ex = &lv_ci_example_list[0]; ex->name != NULL; ex++)
        {
            if (!strcmp(ex->name, argv[3]))
            {
                break;
            }
        }
        if (ex->name == NULL)
        {
            fprintf(stderr, "Unable to find requested example\n");
        }
    }
    lv_init();
    hal_init();
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    init_styles();

    lv_obj_t *obj_cont;
    lv_obj_t *obj_text;

    // ----- Menu & Header -----
    obj_menu = lv_menu_create(lv_scr_act());
    lv_obj_set_size(obj_menu, 240, 320);
    lv_obj_set_style_bg_color(obj_menu, lv_color_hex(color_global_background), 0);
    lv_obj_set_style_bg_opa(obj_menu, LV_OPA_COVER, 0);
    lv_obj_t *menu_header = lv_menu_get_main_header(obj_menu);
    lv_obj_set_height(menu_header, 40);
    lv_obj_set_style_bg_color(menu_header, lv_color_hex(color_header_background), 0);
    lv_obj_set_style_bg_opa(menu_header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(menu_header, lv_color_hex(color_header_titletext), 0);
    lv_obj_set_style_border_side(menu_header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(menu_header, size_borderwidth, 0);
    lv_obj_set_style_border_color(menu_header, lv_color_hex(color_header_border), 0);
    lv_obj_t *menu_back_btn = lv_menu_get_main_header_back_btn(obj_menu);
    lv_obj_set_style_pad_hor(menu_back_btn, 0, 0);
    lv_obj_set_style_pad_ver(menu_back_btn, 9, 0);
    lv_obj_t *arrow_label = lv_obj_get_child(menu_back_btn, 0);
    lv_obj_set_style_text_color(arrow_label, lv_color_hex(color_header_backtext), 0);
    lv_obj_t *menu_back_btn_text = lv_label_create(menu_back_btn);
    lv_label_set_text(menu_back_btn_text, "Back");
    lv_obj_set_style_text_color(menu_back_btn_text, lv_color_hex(color_header_backtext), 0);

    // ----- Page: WiFi Scan -----
    lv_obj_t *obj_page_wifiscan = lv_menu_page_create(obj_menu, "WiFi Scan");
    obj_cont = lv_menu_cont_create(obj_page_wifiscan);
    lv_obj_set_flex_flow(obj_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_t *obj_btn_wifiscan = lv_btn_create(obj_cont);
    lv_obj_add_style(obj_btn_wifiscan, &style_buttonlarge, LV_PART_MAIN | LV_STATE_DEFAULT);
    obj_wifiscan_btnscan_label = lv_label_create(obj_btn_wifiscan);
    lv_label_set_text(obj_wifiscan_btnscan_label, "Start scanning APs");
    lv_obj_add_event_cb(obj_btn_wifiscan, fn_scanaps_eventcb, LV_EVENT_ALL, NULL);
    lv_obj_t *obj_wifiscan_listwifi_wrapper = lv_menu_cont_create(obj_page_wifiscan);
    lv_obj_set_style_pad_hor(obj_wifiscan_listwifi_wrapper, 8, 0);
    lv_obj_set_style_pad_ver(obj_wifiscan_listwifi_wrapper, 0, 0);
    obj_wifiscan_listwifi_cont = lv_menu_cont_create(obj_wifiscan_listwifi_wrapper);
    lv_obj_set_flex_flow(obj_wifiscan_listwifi_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_height(obj_wifiscan_listwifi_cont, 205);
    lv_obj_set_style_bg_color(obj_wifiscan_listwifi_cont, lv_color_hex(0x808080), 0);
    lv_obj_set_style_bg_opa(obj_wifiscan_listwifi_cont, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(obj_wifiscan_listwifi_cont, size_borderwidth, 0);
    lv_obj_set_style_border_color(obj_wifiscan_listwifi_cont, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_side(obj_wifiscan_listwifi_cont, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_border_opa(obj_wifiscan_listwifi_cont, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(obj_wifiscan_listwifi_cont, size_borderradius, 0);
    lv_obj_set_style_pad_row(obj_wifiscan_listwifi_cont, 4, 0);
    lv_obj_set_style_pad_hor(obj_wifiscan_listwifi_cont, 6, 0);
    lv_obj_set_style_pad_ver(obj_wifiscan_listwifi_cont, 6, 0);
    obj_page_wifiinfo = lv_menu_page_create(obj_menu, "AP Info");
    lv_obj_t *obj_wifiscan_info_cont = lv_menu_cont_create(obj_page_wifiinfo);
    lv_obj_set_flex_flow(obj_wifiscan_info_cont, LV_FLEX_FLOW_COLUMN);
    obj_wifiinfo_label = lv_label_create(obj_wifiscan_info_cont);
    lv_obj_set_width(obj_wifiinfo_label, lv_pct(100));
    lv_label_set_long_mode(obj_wifiinfo_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_border_width(obj_wifiinfo_label, size_borderwidth, 0);
    lv_obj_set_style_border_color(obj_wifiinfo_label, lv_color_hex(0x808080), 0);
    lv_obj_set_style_radius(obj_wifiinfo_label, size_borderradius, 0);
    lv_obj_set_style_pad_hor(obj_wifiinfo_label, 8, 0);
    lv_obj_set_style_pad_ver(obj_wifiinfo_label, 8, 0);
    lv_label_set_text(obj_wifiinfo_label, "AP info should be here");
    lv_obj_t *obj_wifiinfo_btnattack = lv_btn_create(obj_wifiscan_info_cont);
    lv_obj_add_style(obj_wifiinfo_btnattack, &style_buttonlarge, LV_PART_MAIN | LV_STATE_DEFAULT);
    obj_text = lv_label_create(obj_wifiinfo_btnattack);
    lv_label_set_text(obj_text, "Attack Captive Portal");
    lv_obj_add_event_cb(obj_wifiinfo_btnattack, fn_captiveportal_eventcb, LV_EVENT_ALL, NULL);
    obj_page_captiveportal = lv_menu_page_create(obj_menu, "Captive Portal");
    lv_obj_t *obj_captiveportal_cont = lv_menu_cont_create(obj_page_captiveportal);
    lv_obj_set_flex_flow(obj_captiveportal_cont, LV_FLEX_FLOW_COLUMN);
    obj_captiveportal_label = lv_label_create(obj_captiveportal_cont);
    lv_label_set_text(obj_captiveportal_label, "Captive Portal text should be here");

    // ----- Page: Keyboard Test -----
    lv_obj_t *obj_page_keyboardtest = lv_menu_page_create(obj_menu, "Keyboard Test");
    obj_cont = lv_menu_cont_create(obj_page_keyboardtest);
    lv_obj_set_flex_flow(obj_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(obj_cont, 6, 0);
    lv_obj_set_style_pad_ver(obj_cont, 6, 0);
    lv_obj_set_style_pad_hor(obj_cont, 8, 0);
    obj_resulttext = lv_textarea_create(obj_cont);
    lv_textarea_set_placeholder_text(obj_resulttext, "Result here...");
    lv_obj_set_size(obj_resulttext, lv_pct(100), 70);
    obj_textinput = lv_textarea_create(obj_cont);
    lv_textarea_set_placeholder_text(obj_textinput, "Tap here to type...");
    lv_obj_set_size(obj_textinput, lv_pct(100), 36);
    lv_obj_add_event_cb(obj_textinput, fn_textinput_eventcb, LV_EVENT_ALL, NULL);
    obj_keyboard = lv_keyboard_create(lv_scr_act());
    lv_obj_add_flag(obj_keyboard, LV_OBJ_FLAG_HIDDEN); // Hide keyboard by default
    lv_keyboard_set_textarea(obj_keyboard, obj_textinput);

    // ----- Page: About Firmware -----
    lv_obj_t *obj_page_about = lv_menu_page_create(obj_menu, "About Firmware");
    obj_cont = lv_menu_cont_create(obj_page_about);
    obj_text = lv_label_create(obj_cont);
    lv_obj_set_width(obj_text, lv_pct(100));
    lv_label_set_long_mode(obj_text, LV_LABEL_LONG_WRAP);
    lv_label_set_text(obj_text, "Firmware V1.0");

    // ----- Page: Main page -----
    lv_obj_t *obj_page_main = lv_menu_page_create(obj_menu, HARDWARE);
    lv_obj_set_style_pad_ver(obj_page_main, 8, 0);
    lv_obj_set_style_pad_hor(obj_page_main, 8, 0);
    lv_obj_set_style_pad_row(obj_page_main, 8, LV_PART_MAIN);
    obj_cont = lv_btn_create(obj_page_main);
    lv_obj_add_style(obj_cont, &style_buttonlarge, LV_PART_MAIN | LV_STATE_DEFAULT);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "WiFi Scan");
    lv_menu_set_load_page_event(obj_menu, obj_cont, obj_page_wifiscan);
    obj_cont = lv_btn_create(obj_page_main);
    lv_obj_add_style(obj_cont, &style_buttonlarge, LV_PART_MAIN | LV_STATE_DEFAULT);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "Keyboard Test");
    lv_menu_set_load_page_event(obj_menu, obj_cont, obj_page_keyboardtest);
    obj_cont = lv_btn_create(obj_page_main);
    lv_obj_add_style(obj_cont, &style_buttonlarge, LV_PART_MAIN | LV_STATE_DEFAULT);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "About Firmware");
    lv_menu_set_load_page_event(obj_menu, obj_cont, obj_page_about);
    lv_menu_set_page(obj_menu, obj_page_main);
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    emscripten_set_main_loop_arg(do_loop, NULL, -1, true);
}
void do_loop(void *arg)
{
    lv_task_handler();
}
static void hal_init(void)
{
    lv_display_t *disp = lv_sdl_window_create(monitor_hor_res, monitor_ver_res);
    lv_group_t *g = lv_group_create();
    lv_group_set_default(g);
    lv_sdl_mouse_create();
    lv_sdl_mousewheel_create();
    lv_sdl_keyboard_create();
    lv_indev_t *mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_group(mousewheel, lv_group_get_default());
    lv_indev_t *keyboard = lv_sdl_keyboard_create();
    lv_indev_set_group(keyboard, lv_group_get_default());
}