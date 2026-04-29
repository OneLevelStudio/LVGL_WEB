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
        lv_obj_remove_flag(obj_keyboard, LV_OBJ_FLAG_HIDDEN);
    }
    // obj_textinput not focused -> hide obj_keyboard
    if (evt_code == LV_EVENT_DEFOCUSED)
    {
        lv_obj_add_flag(obj_keyboard, LV_OBJ_FLAG_HIDDEN);
    }
}

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

    lv_obj_t *obj_menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(obj_menu, 240, 320);
    // lv_obj_set_style_bg_color(obj_menu, lv_palette_lighten(LV_PALETTE_GREY, 1), 0);
    // lv_obj_set_style_bg_opa(obj_menu, LV_OPA_COVER, 0);

    lv_obj_t *menu_header = lv_menu_get_main_header(obj_menu);
    lv_obj_set_height(menu_header, 30);
    lv_obj_set_style_bg_color(menu_header, lv_palette_lighten(LV_PALETTE_GREY, 1), 0);
    lv_obj_set_style_bg_opa(menu_header, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(menu_header, lv_color_black(), 0);
    lv_obj_set_style_border_side(menu_header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(menu_header, 2, 0);

    lv_obj_t *menu_back_btn = lv_menu_get_main_header_back_button(obj_menu);
    lv_obj_t *menu_back_btn_text = lv_label_create(menu_back_btn);
    lv_label_set_text(menu_back_btn_text, "Back");

    // ----- Subpage 1 -----
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
    lv_obj_set_size(obj_resulttext, lv_pct(100), 76);
    // obj_textinput
    obj_textinput = lv_textarea_create(obj_cont);
    lv_textarea_set_placeholder_text(obj_textinput, "Tap here to type...");
    lv_obj_set_size(obj_textinput, lv_pct(100), 36);
    lv_obj_add_event_cb(obj_textinput, fn_textinput_eventcb, LV_EVENT_ALL, NULL);
    // obj_keyboard
    obj_keyboard = lv_keyboard_create(lv_screen_active());
    lv_obj_add_flag(obj_keyboard, LV_OBJ_FLAG_HIDDEN); // Hide obj_keyboard by default
    lv_keyboard_set_textarea(obj_keyboard, obj_textinput);

    // ----- Subpage 2 -----
    lv_obj_t *obj_subpage_2 = lv_menu_page_create(obj_menu, "Page 2");
    obj_cont = lv_menu_cont_create(obj_subpage_2);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "This is page 2");

    // ----- Subpage 3 -----
    lv_obj_t *obj_subpage_3 = lv_menu_page_create(obj_menu, "Page 3");
    obj_cont = lv_menu_cont_create(obj_subpage_3);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "This is page 3");

    // ----- Main page -----
    lv_obj_t *main_page = lv_menu_page_create(obj_menu, "ESP32");
    obj_cont = lv_menu_cont_create(main_page);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "Open Page 1");
    lv_menu_set_load_page_event(obj_menu, obj_cont, obj_subpage_1);
    obj_cont = lv_menu_cont_create(main_page);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "Open Page 2");
    lv_menu_set_load_page_event(obj_menu, obj_cont, obj_subpage_2);
    obj_cont = lv_menu_cont_create(main_page);
    obj_text = lv_label_create(obj_cont);
    lv_label_set_text(obj_text, "Open Page 3");
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