dep_glfw3 = dependency('glfw3', version : '>=3.3')
dep_glm   = dependency('glm', version : '>=0.9.8')
dep_thread = dependency('threads')

borealis_files = files(
    'lib/core/logger.cpp',
    'lib/core/input.cpp',
    'lib/core/application.cpp',
    'lib/core/i18n.cpp',
    'lib/core/theme.cpp',
    'lib/core/style.cpp',
    'lib/core/activity.cpp',
    'lib/core/platform.cpp',
    'lib/core/geometry.cpp',
    'lib/core/font.cpp',
    'lib/core/util.cpp',
    'lib/core/time.cpp',
    'lib/core/timer.cpp',
    'lib/core/animation.cpp',
    'lib/core/task.cpp',
    'lib/core/view.cpp',
    'lib/core/box.cpp',
    'lib/core/bind.cpp',
    'lib/core/thread.cpp',

    'lib/core/gesture.cpp',
    'lib/core/touch/tap_gesture.cpp',
    'lib/core/touch/pan_gesture.cpp',
    'lib/core/touch/scroll_gesture.cpp',

    'lib/platforms/glfw/glfw_platform.cpp',
    'lib/platforms/glfw/glfw_video.cpp',
    'lib/platforms/glfw/glfw_input.cpp',
    'lib/platforms/glfw/glfw_font.cpp',

    'lib/platforms/switch/swkbd.cpp',

    'lib/views/h_scrolling_frame.cpp',
    'lib/views/scrolling_frame.cpp',
    'lib/views/applet_frame.cpp',
    'lib/views/tab_frame.cpp',
    'lib/views/rectangle.cpp',
    'lib/views/recycler.cpp',
    'lib/views/sidebar.cpp',
    'lib/views/label.cpp',
    'lib/views/button.cpp',
    'lib/views/image.cpp',
    'lib/views/header.cpp',
    'lib/views/hint.cpp',
    'lib/views/slider.cpp',
    'lib/views/dropdown.cpp',
    'lib/views/progress_spinner.cpp',
    'lib/views/debug_layer.cpp',
    'lib/views/bottom_bar.cpp',
    'lib/views/dialog.cpp',

    'lib/views/cells/cell_bool.cpp',
    'lib/views/cells/cell_detail.cpp',
    'lib/views/cells/cell_input.cpp',
    'lib/views/cells/cell_radio.cpp',
    'lib/views/cells/cell_selector.cpp',

    'lib/views/widgets/account.cpp',
    'lib/views/widgets/battery.cpp',
    'lib/views/widgets/wireless.cpp',

    'lib/extern/glad/glad.c',
    'lib/extern/nanovg-gl/nanovg.c',
    'lib/extern/tinyxml2/tinyxml2.cpp',

    'lib/extern/fmt/src/format.cc',
    'lib/extern/fmt/src/os.cc',

    'lib/extern/libretro-common/compat/compat_strl.c',
    'lib/extern/libretro-common/features/features_cpu.c',
    'lib/extern/libretro-common/encodings/encoding_utf.c',

    'lib/extern/yoga/src/yoga/event/event.cpp',
    'lib/extern/yoga/src/yoga/log.cpp',
    'lib/extern/yoga/src/yoga/Utils.cpp',
    'lib/extern/yoga/src/yoga/YGConfig.cpp',
    'lib/extern/yoga/src/yoga/YGEnums.cpp',
    'lib/extern/yoga/src/yoga/YGLayout.cpp',
    'lib/extern/yoga/src/yoga/YGNode.cpp',
    'lib/extern/yoga/src/yoga/YGStyle.cpp',
    'lib/extern/yoga/src/yoga/YGValue.cpp',
    'lib/extern/yoga/src/yoga/Yoga.cpp',
)

borealis_include = include_directories(
    'include',
    'lib/extern/fmt/include',
    'include/borealis/extern',
    'include/borealis/extern/tinyxml2',
    'include/borealis/extern/nanovg-gl',
    'lib/extern/yoga/src',
    'lib/extern/tweeny/include',
)

borealis_dependencies = [ dep_glfw3, dep_glm, dep_thread, ]
borealis_cpp_args = [ '-DNO_TOUCH_SCROLLING=true', '-DYG_ENABLE_EVENTS', '-D__GLFW__', ]
