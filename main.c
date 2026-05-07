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
static lv_style_t style_labelinfobox;

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

    lv_style_init(&style_labelinfobox);
    lv_style_set_width(&style_labelinfobox, lv_pct(100));
    lv_style_set_border_width(&style_labelinfobox, size_borderwidth);
    lv_style_set_border_color(&style_labelinfobox, lv_color_hex(0x808080));
    lv_style_set_radius(&style_labelinfobox, size_borderradius);
    lv_style_set_pad_left(&style_labelinfobox, 8);
    lv_style_set_pad_right(&style_labelinfobox, 8);
    lv_style_set_pad_top(&style_labelinfobox, 8);
    lv_style_set_pad_bottom(&style_labelinfobox, 8);
}

// ---------- Page: Keyboard Test ----------
static lv_obj_t *kbtest_txarea_result;
static lv_obj_t *kbtest_txarea_input;
static lv_obj_t *kbtest_keyboard;
static void fn_textinput_eventcb(lv_event_t *evt)
{
    lv_event_code_t evt_code = lv_event_get_code(evt);
    // Keyboard OK/SUBMIT button pressed -> kbtest_txarea_result
    if (evt_code == LV_EVENT_READY)
    {
        const char *newtext = lv_textarea_get_text(kbtest_txarea_input);
        lv_textarea_set_text(kbtest_txarea_result, newtext);
    }
    // kbtest_txarea_input focused -> show kbtest_keyboard
    if (evt_code == LV_EVENT_FOCUSED)
    {
        lv_obj_clear_flag(kbtest_keyboard, LV_OBJ_FLAG_HIDDEN);
    }
    // kbtest_txarea_input not focused -> hide kbtest_keyboard
    if (evt_code == LV_EVENT_DEFOCUSED)
    {
        lv_obj_add_flag(kbtest_keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

// ---------- Page: WiFi Hijacking ----------
static lv_obj_t *page_wifiinfo;
static lv_obj_t *page_eviltwin;
static lv_obj_t *btn_wifiscan_label;
static lv_obj_t *wifiscan_list;
static lv_obj_t *wifiinfo_label;
static lv_obj_t *eviltwin_label;
// static String saved_ssid_for_eviltwin = "Free WiFi";
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
    // saved_ssid_for_eviltwin = WiFi.SSID(idx);
    // uint8_t *bssid = WiFi.BSSID(idx);
    // char strbuf_bssid[18] = "";
    // if (bssid)
    // {
    //     snprintf(strbuf_bssid, sizeof(strbuf_bssid), "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    // }
    // char strbuf[256];
    // snprintf(strbuf, sizeof(strbuf), "SSID: %s\nRSSI: %d dBm\nChannel: %d\nBSSID: %s\nEncryption Type: %s", saved_ssid_for_eviltwin.c_str(), WiFi.RSSI(idx), WiFi.channel(idx), strbuf_bssid, get_wifi_encryption_type(WiFi.encryptionType(idx)));
    // // --------------------------------------------------
    char strbuf[256];
    snprintf(strbuf, sizeof(strbuf), "SSID: WiFi Network #%d\nRSSI: 12345 dBm\nChannel: 123\nBSSID: 12-34-56-78-89\nEncryption Type: ABC", idx);
    // // --------------------------------------------------

    lv_label_set_text(wifiinfo_label, strbuf);
    lv_menu_set_page(obj_menu, page_wifiinfo);
}
static void fn_scanaps_eventcb(lv_event_t *evt)
{
    lv_event_code_t evt_code = lv_event_get_code(evt);
    // btn_wifiscan clicked
    if (evt_code == LV_EVENT_CLICKED)
    {
        lv_obj_clean(wifiscan_list);
        lv_label_set_text(btn_wifiscan_label, "Scanning...");
        lv_refr_now(NULL);

        // // --------------------------------------------------
        // WiFi.mode(WIFI_STA);
        // WiFi.disconnect();
        // delay(100);
        // int n_aps = WiFi.scanNetworks(false, true); // (async, show_hidden)
        // if (n_aps < 0)
        // {
        //     lv_label_set_text(btn_wifiscan_label, "Status: Failed");
        //     return;
        // }
        // for (int idx = 0; idx < n_aps; ++idx)
        // {
        //     lv_obj_t *obj_btn_item_ap = lv_btn_create(wifiscan_list);
        //     lv_obj_add_style(obj_btn_item_ap, &style_buttonsmall, LV_PART_MAIN | LV_STATE_DEFAULT);
        //     lv_obj_t *obj_btn_item_ap_label = lv_label_create(obj_btn_item_ap);
        //     char strbuf[96];
        //     snprintf(strbuf, sizeof(strbuf), "%s [%s] (%d dBm)", WiFi.SSID(idx).c_str(), get_wifi_encryption_type(WiFi.encryptionType(idx)), WiFi.RSSI(idx));
        //     lv_label_set_text(obj_btn_item_ap_label, strbuf);
        //     lv_obj_add_event_cb(obj_btn_item_ap, fn_apdetailpage_eventcb, LV_EVENT_CLICKED, (void *)(intptr_t)idx);
        // }
        // // --------------------------------------------------
        int n_aps = 10;
        for (int idx = 0; idx < n_aps; ++idx)
        {
            lv_obj_t *obj_btn_item_ap = lv_btn_create(wifiscan_list);
            lv_obj_add_style(obj_btn_item_ap, &style_buttonsmall, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_t *obj_btn_item_ap_label = lv_label_create(obj_btn_item_ap);
            char strbuf[96];
            snprintf(strbuf, sizeof(strbuf), "Dummy Wifi #%d", idx);
            lv_label_set_text(obj_btn_item_ap_label, strbuf);
            lv_obj_add_event_cb(obj_btn_item_ap, fn_apdetailpage_eventcb, LV_EVENT_CLICKED, (void *)(intptr_t)idx);
        }
        // // --------------------------------------------------

        char strbuf[64];
        snprintf(strbuf, sizeof(strbuf), "Status: %d AP(s) found", n_aps);
        lv_label_set_text(btn_wifiscan_label, strbuf);
    }
}

// void send_eviltwin()
// {
//     String portal_html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body><h2>Evil Twin is working</h2></body></html>";
//     webServer.send(200, "text/html", portal_html);
// }
static void fn_eviltwin_eventcb(lv_event_t *evt)
{
    lv_event_code_t evt_code = lv_event_get_code(evt);
    // btn_eviltwin clicked
    if (evt_code == LV_EVENT_CLICKED)
    {
        lv_menu_set_page(obj_menu, page_eviltwin);

        // // --------------------------------------------------
        // WiFi.mode(WIFI_AP);
        // WiFi.disconnect();
        // delay(100);
        // WiFi.softAP(saved_ssid_for_eviltwin + " (Test)");
        // dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
        // webServer.on("/", send_eviltwin);
        // webServer.onNotFound(send_eviltwin);
        // webServer.begin();
        // lv_label_set_text(eviltwin_label, "Evil Twin '" + saved_ssid_for_eviltwin + "' deployed.");
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
    lv_obj_t *menu_backbtn = lv_menu_get_main_header_back_btn(obj_menu);
    lv_obj_set_style_pad_hor(menu_backbtn, 0, 0);
    lv_obj_set_style_pad_ver(menu_backbtn, 8, 0);
    lv_obj_t *menu_backbtn_arrow = lv_obj_get_child(menu_backbtn, 0);
    lv_obj_set_style_text_color(menu_backbtn_arrow, lv_color_hex(color_header_backtext), 0);
    lv_obj_t *menu_backbtn_label = lv_label_create(menu_backbtn);
    lv_label_set_text(menu_backbtn_label, "Back");
    lv_obj_set_style_text_color(menu_backbtn_label, lv_color_hex(color_header_backtext), 0);

    // ----- Page: WiFi Hijacking -----
    lv_obj_t *page_wifihijack = lv_menu_page_create(obj_menu, "Network Scanning");
    obj_cont = lv_menu_cont_create(page_wifihijack);
    lv_obj_set_flex_flow(obj_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_t *btn_wifiscan = lv_btn_create(obj_cont);
    lv_obj_add_style(btn_wifiscan, &style_buttonlarge, LV_PART_MAIN | LV_STATE_DEFAULT);
    btn_wifiscan_label = lv_label_create(btn_wifiscan);
    lv_label_set_text(btn_wifiscan_label, "Scan APs");
    lv_obj_center(btn_wifiscan_label);
    lv_obj_add_event_cb(btn_wifiscan, fn_scanaps_eventcb, LV_EVENT_ALL, NULL);
    lv_obj_t *wifiscan_list_wrapper = lv_menu_cont_create(page_wifihijack);
    lv_obj_set_style_pad_hor(wifiscan_list_wrapper, 8, 0);
    lv_obj_set_style_pad_ver(wifiscan_list_wrapper, 0, 0);
    wifiscan_list = lv_menu_cont_create(wifiscan_list_wrapper);
    lv_obj_set_flex_flow(wifiscan_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_height(wifiscan_list, 205);
    lv_obj_set_style_bg_color(wifiscan_list, lv_color_hex(0x808080), 0);
    lv_obj_set_style_bg_opa(wifiscan_list, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(wifiscan_list, size_borderwidth, 0);
    lv_obj_set_style_border_color(wifiscan_list, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_border_side(wifiscan_list, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_border_opa(wifiscan_list, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(wifiscan_list, size_borderradius, 0);
    lv_obj_set_style_pad_row(wifiscan_list, 4, 0);
    lv_obj_set_style_pad_hor(wifiscan_list, 6, 0);
    lv_obj_set_style_pad_ver(wifiscan_list, 6, 0);
    page_wifiinfo = lv_menu_page_create(obj_menu, "AP Info");
    lv_obj_t *wifiinfo_cont = lv_menu_cont_create(page_wifiinfo);
    lv_obj_set_flex_flow(wifiinfo_cont, LV_FLEX_FLOW_COLUMN);
    wifiinfo_label = lv_label_create(wifiinfo_cont);
    lv_label_set_long_mode(wifiinfo_label, LV_LABEL_LONG_WRAP);
    lv_obj_add_style(wifiinfo_label, &style_labelinfobox, 0);
    lv_label_set_text(wifiinfo_label, "[Init text] AP Info should be here");
    lv_obj_t *btn_eviltwin = lv_btn_create(wifiinfo_cont);
    lv_obj_add_style(btn_eviltwin, &style_buttonlarge, LV_PART_MAIN | LV_STATE_DEFAULT);
    obj_text = lv_label_create(btn_eviltwin);
    lv_label_set_text(obj_text, "Deploy Evil Twin");
    lv_obj_add_event_cb(btn_eviltwin, fn_eviltwin_eventcb, LV_EVENT_ALL, NULL);
    page_eviltwin = lv_menu_page_create(obj_menu, "Evil Twin");
    lv_obj_t *eviltwin_cont = lv_menu_cont_create(page_eviltwin);
    lv_obj_set_flex_flow(eviltwin_cont, LV_FLEX_FLOW_COLUMN);
    eviltwin_label = lv_label_create(eviltwin_cont);
    lv_label_set_text(eviltwin_label, "[Init text] Evil Twin should be here");
    lv_obj_add_style(eviltwin_label, &style_labelinfobox, 0);
    lv_label_set_long_mode(eviltwin_label, LV_LABEL_LONG_WRAP);

    // ----- Page: Keyboard Test -----
    lv_obj_t *page_keyboardtest = lv_menu_page_create(obj_menu, "Keyboard Test");
    obj_cont = lv_menu_cont_create(page_keyboardtest);
    lv_obj_set_flex_flow(obj_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(obj_cont, 6, 0);
    lv_obj_set_style_pad_ver(obj_cont, 6, 0);
    lv_obj_set_style_pad_hor(obj_cont, 8, 0);
    kbtest_txarea_result = lv_textarea_create(obj_cont);
    lv_textarea_set_placeholder_text(kbtest_txarea_result, "Result here...");
    lv_obj_set_size(kbtest_txarea_result, lv_pct(100), 65);
    kbtest_txarea_input = lv_textarea_create(obj_cont);
    lv_textarea_set_placeholder_text(kbtest_txarea_input, "Tap here to type...");
    lv_obj_set_size(kbtest_txarea_input, lv_pct(100), 37);
    lv_obj_add_event_cb(kbtest_txarea_input, fn_textinput_eventcb, LV_EVENT_ALL, NULL);
    kbtest_keyboard = lv_keyboard_create(lv_scr_act());
    lv_obj_add_flag(kbtest_keyboard, LV_OBJ_FLAG_HIDDEN); // Hide keyboard by default
    lv_keyboard_set_textarea(kbtest_keyboard, kbtest_txarea_input);

    // ----- Page: About -----
    lv_obj_t *page_about = lv_menu_page_create(obj_menu, "About");
    obj_cont = lv_menu_cont_create(page_about);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_long_mode(obj_text, LV_LABEL_LONG_WRAP);
    lv_obj_add_style(obj_text, &style_labelinfobox, 0);
    lv_label_set_text(obj_text, "Firmware V1.0");

    // ----- Page: Main page -----
    lv_obj_t *page_main = lv_menu_page_create(obj_menu, HARDWARE);
    lv_obj_set_style_pad_ver(page_main, 8, 0);
    lv_obj_set_style_pad_hor(page_main, 8, 0);
    lv_obj_set_style_pad_row(page_main, 8, LV_PART_MAIN);
    obj_cont = lv_btn_create(page_main);
    lv_obj_add_style(obj_cont, &style_buttonlarge, LV_PART_MAIN | LV_STATE_DEFAULT);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "WiFi Hijacking");
    lv_menu_set_load_page_event(obj_menu, obj_cont, page_wifihijack);
    obj_cont = lv_btn_create(page_main);
    lv_obj_add_style(obj_cont, &style_buttonlarge, LV_PART_MAIN | LV_STATE_DEFAULT);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "Keyboard Test");
    lv_menu_set_load_page_event(obj_menu, obj_cont, page_keyboardtest);
    obj_cont = lv_btn_create(page_main);
    lv_obj_add_style(obj_cont, &style_buttonlarge, LV_PART_MAIN | LV_STATE_DEFAULT);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "About");
    lv_menu_set_load_page_event(obj_menu, obj_cont, page_about);
    lv_menu_set_page(obj_menu, page_main);
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