#include "display.h"
#include <lvgl.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/battery_state_changed.h>

static lv_obj_t *device_labels[3];
static lv_obj_t *layer_label;
static lv_obj_t *battery_label;

static void update_device_display(uint8_t active_device) {
    for (int i = 0; i < 3; i++) {
        lv_label_set_text_fmt(device_labels[i], "%s", (i == active_device) ? "[●]" : "[○]");
    }
}

static int layer_event_listener(const struct zmk_event_header *eh) {
    struct layer_state_changed *event = cast_layer_state_changed(eh);
    if (event->layer_state & (1 << FN_LAYER)) {
        lv_label_set_text(layer_label, "FN");
    } else {
        lv_label_set_text(layer_label, "");
    }
    return 0;
}

static int battery_event_listener(const struct zmk_event_header *eh) {
    struct battery_state_changed *event = cast_battery_state_changed(eh);
    lv_label_set_text_fmt(battery_label, "Battery: %d%%", event->percentage);
    return 0;
}

void display_init(void) {
    lv_obj_t *screen = lv_scr_act();

    // Devices 1 2 3
    for (int i = 0; i < 3; i++) {
        device_labels[i] = lv_label_create(screen);
        lv_obj_set_pos(device_labels[i], i * 20, 0);
    }
    update_device_display(0); // initially device 1 active

    // Layer label
    layer_label = lv_label_create(screen);
    lv_obj_set_pos(layer_label, 0, 20);
    lv_label_set_text(layer_label, "");

    // Battery label
    battery_label = lv_label_create(screen);
    lv_obj_set_pos(battery_label, 0, 40);
    lv_label_set_text(battery_label, "Battery: --%");

    // Register listeners
    ZMK_LISTENER(layer_listener, layer_event_listener);
    ZMK_SUBSCRIBE(layer_listener, layer_state_changed);

    ZMK_LISTENER(battery_listener, battery_event_listener);
    ZMK_SUBSCRIBE(battery_listener, battery_state_changed);
}
