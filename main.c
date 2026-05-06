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
static lv_obj_t *obj_menu;

// ---------- Page 1 ----------
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

// ---------- Page 2 ----------
static lv_obj_t *obj_scanaps_status;
static lv_obj_t *obj_scanaps_list;
static lv_obj_t *obj_scanaps_detailpage;
static lv_obj_t *obj_scanaps_detail_cont_label;
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
    // uint8_t *bssid = WiFi.BSSID(idx);
    // char bssidStr[18] = "";
    // if (bssid)
    // {
    //     snprintf(bssidStr, sizeof(bssidStr), "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
    // }
    // char temp_str_buf[256];
    // snprintf(temp_str_buf, sizeof(temp_str_buf), "SSID: %s\nRSSI: %d dBm\nChannel: %d\nBSSID: %s\nEncryption Type: %s", WiFi.SSID(idx).c_str(), WiFi.RSSI(idx), WiFi.channel(idx), bssidStr, get_wifi_encryption_type(WiFi.encryptionType(idx)));
    // // --------------------------------------------------
    char temp_str_buf[256];
    snprintf(temp_str_buf, sizeof(temp_str_buf), "SSID: WiFi Network #%d\nRSSI: 12345 dBm\nChannel: 123\nBSSID: 12-34-56-78-89\nEncryption Type: ABC", idx);
    // // --------------------------------------------------

    lv_label_set_text(obj_scanaps_detail_cont_label, temp_str_buf);
    if (obj_menu && obj_scanaps_detailpage)
    {
        lv_menu_set_page(obj_menu, obj_scanaps_detailpage);
    }
}
static void fn_scanaps_eventcb(lv_event_t *evt)
{
    lv_event_code_t evt_code = lv_event_get_code(evt);
    // obj_btn_scanaps clicked
    if (evt_code == LV_EVENT_CLICKED)
    {
        lv_obj_clean(obj_scanaps_list);
        lv_label_set_text(obj_scanaps_status, "Scan APs Status: Scanning...");
        lv_refr_now(NULL);

        // // --------------------------------------------------
        // WiFi.mode(WIFI_STA);
        // WiFi.disconnect();
        // delay(100);
        // int n_aps = WiFi.scanNetworks(false, true); // (async, show_hidden)
        // if (n_aps < 0)
        // {
        //     lv_label_set_text(obj_scanaps_status, "Scan APs Status: Failed");
        //     return;
        // }
        // for (int idx = 0; idx < n_aps; ++idx)
        // {
        //     lv_obj_t *obj_btn_item_ap = lv_btn_create(obj_scanaps_list);
        //     lv_obj_set_width(obj_btn_item_ap, lv_pct(100));
        //     lv_obj_t *obj_btn_item_ap_label = lv_label_create(obj_btn_item_ap);
        //     char str_btn_item_ap_label[96];
        //     snprintf(str_btn_item_ap_label, sizeof(str_btn_item_ap_label), "%s [%s] (%d dBm)", WiFi.SSID(idx).c_str(), get_wifi_encryption_type(WiFi.encryptionType(idx)), WiFi.RSSI(idx));
        //     lv_label_set_text(obj_btn_item_ap_label, str_btn_item_ap_label);
        //     lv_obj_add_event_cb(obj_btn_item_ap, fn_apdetailpage_eventcb, LV_EVENT_CLICKED, (void *)(intptr_t)idx);
        // }
        // // --------------------------------------------------
        usleep(1000000); // = delay(1000);
        int n_aps = 10;
        for (int idx = 0; idx < n_aps; ++idx)
        {
            lv_obj_t *obj_btn_item_ap = lv_btn_create(obj_scanaps_list);
            lv_obj_set_width(obj_btn_item_ap, lv_pct(100));
            lv_obj_t *obj_btn_item_ap_label = lv_label_create(obj_btn_item_ap);
            char str_btn_item_ap_label[96];
            snprintf(str_btn_item_ap_label, sizeof(str_btn_item_ap_label), "Wifi Network #%d [%s] (%d dBm)", idx, "Encryption Type", 12345);
            lv_label_set_text(obj_btn_item_ap_label, str_btn_item_ap_label);
            lv_obj_add_event_cb(obj_btn_item_ap, fn_apdetailpage_eventcb, LV_EVENT_CLICKED, (void *)(intptr_t)idx);
        }
        // // --------------------------------------------------

        char temp_str_buf[64];
        snprintf(temp_str_buf, sizeof(temp_str_buf), "Scan APs Status: %d found.", n_aps);
        lv_label_set_text(obj_scanaps_status, temp_str_buf);
    }
}

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
    lv_obj_t *obj_cont;
    lv_obj_t *obj_text;

    obj_menu = lv_menu_create(lv_scr_act());
    lv_obj_set_size(obj_menu, 240, 320);
    lv_obj_set_style_bg_color(obj_menu, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(obj_menu, LV_OPA_COVER, 0);

    lv_obj_t *menu_header = lv_menu_get_main_header(obj_menu);
    lv_obj_set_height(menu_header, 36);
    lv_obj_set_style_bg_color(menu_header, lv_color_hex(0x222222), 0);
    lv_obj_set_style_bg_opa(menu_header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(menu_header, lv_palette_main(LV_PALETTE_LIGHT_GREEN), 0);
    lv_obj_set_style_border_side(menu_header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(menu_header, 2, 0);
    lv_obj_set_style_border_color(menu_header, lv_palette_main(LV_PALETTE_TEAL), 0);

    lv_obj_t *menu_back_btn = lv_menu_get_main_header_back_btn(obj_menu);
    lv_obj_t *menu_back_btn_text = lv_label_create(menu_back_btn);
    lv_label_set_text(menu_back_btn_text, "Back");

    // ----- Page 1 -----
    lv_obj_t *obj_subpage_1 = lv_menu_page_create(obj_menu, "Page 1");
    obj_cont = lv_menu_cont_create(obj_subpage_1);
    lv_obj_set_flex_flow(obj_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(obj_cont, 6, 0);
    lv_obj_set_style_pad_ver(obj_cont, 6, 0);
    lv_obj_set_style_pad_hor(obj_cont, 8, 0);
    // obj_resulttext
    obj_resulttext = lv_textarea_create(obj_cont);
    lv_textarea_set_placeholder_text(obj_resulttext, "Result here...");
    lv_obj_set_size(obj_resulttext, lv_pct(100), 72);
    // obj_textinput
    obj_textinput = lv_textarea_create(obj_cont);
    lv_textarea_set_placeholder_text(obj_textinput, "Tap here to type...");
    lv_obj_set_size(obj_textinput, lv_pct(100), 40);
    lv_obj_add_event_cb(obj_textinput, fn_textinput_eventcb, LV_EVENT_ALL, NULL);
    // obj_keyboard
    obj_keyboard = lv_keyboard_create(lv_scr_act());
    lv_obj_add_flag(obj_keyboard, LV_OBJ_FLAG_HIDDEN); // Hide obj_keyboard by default
    lv_keyboard_set_textarea(obj_keyboard, obj_textinput);

    // ----- Page 2 -----
    lv_obj_t *obj_subpage_2 = lv_menu_page_create(obj_menu, "Page 2");
    obj_cont = lv_menu_cont_create(obj_subpage_2);
    lv_obj_set_flex_flow(obj_cont, LV_FLEX_FLOW_COLUMN);
    // obj_btn_scanaps
    lv_obj_t *obj_btn_scanaps = lv_btn_create(obj_cont);
    lv_obj_t *obj_btn_scanaps_label = lv_label_create(obj_btn_scanaps);
    lv_label_set_text(obj_btn_scanaps_label, "Scan APs");
    lv_obj_add_event_cb(obj_btn_scanaps, fn_scanaps_eventcb, LV_EVENT_ALL, NULL);
    // obj_scanaps_status
    obj_scanaps_status = lv_label_create(obj_cont);
    lv_label_set_text(obj_scanaps_status, "Scan APs Status: Idle");
    // obj_scanaps_list
    obj_scanaps_list = lv_menu_cont_create(obj_subpage_2);
    lv_obj_set_flex_flow(obj_scanaps_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_height(obj_scanaps_list, 150);
    // obj_scanaps_detailpage
    obj_scanaps_detailpage = lv_menu_page_create(obj_menu, "AP Details");
    lv_obj_t *obj_scanaps_detail_cont = lv_menu_cont_create(obj_scanaps_detailpage);
    lv_obj_set_flex_flow(obj_scanaps_detail_cont, LV_FLEX_FLOW_COLUMN);
    obj_scanaps_detail_cont_label = lv_label_create(obj_scanaps_detail_cont);
    lv_obj_set_width(obj_scanaps_detail_cont_label, lv_pct(100));
    lv_label_set_long_mode(obj_scanaps_detail_cont_label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(obj_scanaps_detail_cont_label, "AP details info should be here.");

    // ----- Page 3 -----
    lv_obj_t *obj_subpage_3 = lv_menu_page_create(obj_menu, "Page 3");
    obj_cont = lv_menu_cont_create(obj_subpage_3);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "This is the content of page 3");

    // ----- Main page -----
    lv_obj_t *main_page = lv_menu_page_create(obj_menu, HARDWARE);
    obj_cont = lv_menu_cont_create(main_page);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "Page 1 - Test Keyboard");
    lv_menu_set_load_page_event(obj_menu, obj_cont, obj_subpage_1);
    obj_cont = lv_menu_cont_create(main_page);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "Page 2 - Scan APs");
    lv_menu_set_load_page_event(obj_menu, obj_cont, obj_subpage_2);
    obj_cont = lv_menu_cont_create(main_page);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "Page 3 - None");
    lv_menu_set_load_page_event(obj_menu, obj_cont, obj_subpage_3);
    lv_menu_set_page(obj_menu, main_page);
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    // ====================================================================================================
    emscripten_set_main_loop_arg(do_loop, NULL, -1, true);
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