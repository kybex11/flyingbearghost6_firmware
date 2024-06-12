#define TABLE_SIZE_X                 210
#define TABLE_SIZE_Y                 210
#define TABLE_SIZE_Z                 250

#define MAX_TABLE_TEMP               110
#define MAX_HOTEND_TEMP              260

#define START_COORD_X                0
#define START_COORD_Y                0
#define START_COORD_Z                0

#define USE_BL_TOUCH                 false

#define CURRENT_COORD_X              0
#define CURRENT_COORD_Y              0
#define CURRENT_COORD_Z              0

#define ENDSTOP_SYS_ON               true

#define ENDSTOP_X                    false
#define ENDSTOP_Y                    false
#define ENDSTOP_Z                    false

#define AUTO_SCAN_COORD_AFTER_LAUNCH true

#define HOTEND_HEATING_TEMP          0
#define BED_HEATING_TEMP             0

// don't recommended change the defines 
#define STEP_PIN            2
#define DIR_PIN             3
#define STEP_MAX_SPEED      150
#define STEP_ACCELERATION   75
#define TFT_CS              10
#define TFT_RST             9
#define TFT_DC              8
#define MAX_TEMP            100

//device
#define BCM2708_PERI_BASE           0x3F000000
#define GPIO_BASE                   (BCM2708_PERI_BASE + 0x200000)

//macro
#define SET_MAX_TEMP(temp) ((temp >MAX_TEMP) ? MAX_TEMP : temp)

//menu
#define MENU_SELECTED_BED_TEMP      SET_MAX_TEMP(MAX_TABLE_TEMP)
#define MENU_SELECTED_HOTEND_TEMP   SET_MAX_TEMP(MAX_HOTEND_TEMP)
