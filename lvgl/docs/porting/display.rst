.. _display_interface:

=================
Display interface
=================

To create a display for LVGL call
:cpp:expr:`lv_display_t * display = lv_display_create(hor_res, ver_res)`. You can create
a multiple displays and a different driver for each (see below),

Basic setup
***********

Draw buffer(s) are simple array(s) that LVGL uses to render the screen's
content. Once rendering is ready the content of the draw buffer is sent
to the display using the ``flush_cb`` function.

flush_cb
--------

An example ``flush_cb`` looks like this:

.. code-block:: c

   void my_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
   {
       /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
        *`put_px` is just an example, it needs to be implemented by you.*/
       uint16_t * buf16 = (uint16_t *)px_map; /*Let's say it's a 16 bit (RGB565) display*/
       int32_t x, y;
       for(y = area->y1; y <= area->y2; y++) {
           for(x = area->x1; x <= area->x2; x++) {
               put_px(x, y, *buf16);
               buf16++;
           }
       }

       /* IMPORTANT!!!
        * Inform LVGL that you are ready with the flushing and buf is not used anymore*/
       lv_display_flush_ready(disp);
   }

Use :cpp:expr:`lv_display_set_flush_cb(disp, my_flush_cb)` to set a new ``flush_cb``.

:cpp:expr:`lv_display_flush_ready(disp)` needs to be called when flushing is ready
to inform LVGL the buffer is not used anymore by the driver and it can
render new content into it.

LVGL might render the screen in multiple chunks and therefore call
``flush_cb`` multiple times. To see if the current one is the last chunk
of rendering use :cpp:expr:`lv_display_flush_is_last(display)`.

Draw buffers
------------

The draw buffers can be set with
:cpp:expr:`lv_display_set_buffers(display, buf1, buf2, buf_size_byte, render_mode)`

-  ``buf1`` a buffer where LVGL can render
-  ``buf2`` a second optional buffer (see more details below)
-  ``buf_size_byte`` size of the buffer(s) in bytes
-  ``render_mode``

   -  :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL` Use the buffer(s) to render the
      screen in smaller parts. This way the buffers can be smaller then
      the display to save RAM. At least 1/10 screen size buffer(s) are
      recommended. In ``flush_cb`` the rendered images needs to be
      copied to the given area of the display. In this mode if a button is pressed
      only the button's area will be redrawn.
   -  :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT` The buffer(s) has to be screen
      sized and LVGL will render into the correct location of the
      buffer. This way the buffer always contain the whole image. If two
      buffer are used the rendered areas are automatically copied to the
      other buffer after flushing. Due to this in ``flush_cb`` typically
      only a frame buffer address needs to be changed. If a button is pressed
      only the button's area will be redrawn.
   -  :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_FULL` The buffer(s) has to be screen
      sized and LVGL will always redraw the whole screen even if only 1
      pixel has been changed. If two screen sized draw buffers are
      provided, LVGL's display handling works like "traditional" double
      buffering. This means the ``flush_cb`` callback only has to update
      the address of the frame buffer to the ``px_map`` parameter.

Example:

.. code-block:: c

   static uint16_t buf[LCD_HOR_RES * LCD_VER_RES / 10];
   lv_display_set_buffers(disp, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

One buffer
^^^^^^^^^^

If only one buffer is used LVGL draws the content of the screen into
that draw buffer and sends it to the display via the ``flush_cb``. LVGL
then needs to wait until :cpp:expr:`lv_display_flush_ready` is called
(that is the content of the buffer is sent to the
display) before drawing something new into it.

Two buffers
^^^^^^^^^^^

If two buffers are used LVGL can draw into one buffer while the content
of the other buffer is sent to the display in the background. DMA or
other hardware should be used to transfer data to the display so the MCU
can continue drawing. This way, the rendering and refreshing of the
display become parallel operations.

Advanced options
****************

Resolution
----------

To set the resolution of the display after creation use
:cpp:expr:`lv_display_set_resolution(display, hor_res, ver_res)`

It's not mandatory to use the whole display for LVGL, however in some
cases the physical resolution is important. For example the touchpad
still sees the whole resolution and the values needs to be converted to
the active LVGL display area. So the physical resolution and the offset
of the active area can be set with
:cpp:expr:`lv_display_set_physical_resolution(disp, hor_res, ver_res)` and
:cpp:expr:`lv_display_set_offset(disp, x, y)`

Flush wait callback
-------------------

By using :cpp:expr:`lv_display_flush_ready` LVGL will spin in a loop
while waiting for flushing.

However with the help of :cpp:expr:`lv_display_set_flush_wait_cb` a custom
wait callback be set for flushing. This callback can use a semaphore, mutex,
or anything else to optimize while the waiting for flush.

If ``flush_wait_cb`` is not set, LVGL assume that `lv_display_flush_ready`
is used.

Rotation
--------

LVGL supports rotation of the display in 90 degree increments. You can
select whether you would like software rotation or hardware rotation.

The orientation of the display can be changed with
``lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0/90/180/270)``.
LVGL will swap the horizontal and vertical resolutions internally
according to the set degree. When changing the rotation
:cpp:expr:`LV_EVENT_SIZE_CHANGED` is sent to the display to allow
reconfiguring the hardware. In lack of hardware display rotation support
:cpp:expr:`lv_draw_sw_rotate` can be used to rotate the buffer in the
``flush_cb``.

:cpp:expr:`lv_display_rotate_area(display, &area)` rotates the rendered area
according to the current rotation settings of the display.

Note that in :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT` the small changed areas
are rendered directly in the frame buffer so they cannot be
rotated later. Therefore in direct mode only the whole frame buffer can be rotated.
The same is true for :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_FULL`.

In the case of :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL` the small rendered areas
can be rotated on their own before flushing to the frame buffer.

Color format
------------

The default color format of the display is set according to :c:macro:`LV_COLOR_DEPTH`
(see ``lv_conf.h``)

- :c:macro:`LV_COLOR_DEPTH` ``32``: XRGB8888 (4 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``24``: RGB888 (3 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``16``: RGB565 (2 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``8``: L8 (1 bytes/pixel)
- :c:macro:`LV_COLOR_DEPTH` ``1``: I1 (1 bit/pixel) Only support for horizontal mapped buffers. See :ref:`monochrome` for more details:

The ``color_format`` can be changed with
``lv_display_set_color_depth(display, LV_COLOR_FORMAT_...)`` .
Besides the default value :c:macro:`LV_COLOR_FORMAT_ARGB8888` can be
used as a well.

It's very important that draw buffer(s) should be large enough for any
selected color format.


Swap endianness
---------------

In case of RGB565 color format it might be required to swap the 2 bytes
because the SPI, I2C or 8 bit parallel port periphery sends them in the wrong order.

The ideal solution is configure the hardware to handle the 16 bit data with different byte order,
however if it's not possible :cpp:expr:`lv_draw_sw_rgb565_swap(buf, buf_size_in_px)`
can be called in the ``flush_cb`` to swap the bytes.

If you wish you can also write your own function, or use assembly instructions for
the fastest possible byte swapping.

Note that this is not about swapping the Red and Blue channel but converting

``RRRRR GGG | GGG BBBBB``

to

``GGG BBBBB | RRRRR GGG``.

.. _monochrome:

Monochrome Displays
-------------------

LVGL supports rendering directly in a 1-bit format for monochrome displays.
To enable it, set ``LV_COLOR_DEPTH 1`` or use :cpp:expr:`lv_display_set_color_format(display, LV_COLOR_FORMAT_I1)`.

The :cpp:expr:`LV_COLOR_FORMAT_I1` format assumes that bytes are mapped to rows (i.e., the bits of a byte are written next to each other).
The order of bits is MSB first, which means:

.. code::

             MSB           LSB
   bits       7 6 5 4 3 2 1 0
   pixels     0 1 2 3 4 5 6 7
             Left         Right

Ensure that the LCD controller is configured accordingly.

Internally, LVGL rounds the redrawn areas to byte boundaries. Therefore, updated areas will:

- Start on an ``Nx8`` coordinate.
- End on an ``Nx8 - 1`` coordinate.

When setting up the buffers for rendering (:cpp:func:`lv_display_set_buffers`), make the buffer 8 bytes larger.
This is necessary because LVGL reserves 2 x 4 bytes in the buffer, as these are assumed to be used as a palette.

To skip the palette, include the following line in your ``flush_cb`` function: ``px_map += 8``.

As usual, monochrome displays support partial, full, and direct rendering modes as well.
In full and direct modes, the buffer size should be large enough for the whole screen, meaning ``(horizontal_resolution x vertical_resolution / 8) + 8`` bytes.
As LVGL can not handle fractional width make sure to round the horizontal resolution to 8-
(For example 90 to 96)

Constraints on the Redrawn Area
-------------------------------

Some display controllers have specific requirements for the window area where the rendered image can be sent
(e.g., `x1` must be even, and `x2` must be odd).

In the case of monochrome displays, `x1` must be `Nx8`, and `x2` must be `Nx8 - 1`.
(If the display uses `LV_COLOR_FORMAT_I1`, LVGL automatically applies this rounding. See :ref:`monochrome`.)

The size of the invalidated (redrawn) area can be controlled as follows:

.. code:: c

    void rounder_event_cb(lv_event_t * e)
    {
        lv_area_t * a = lv_event_get_invalidated_area(e);

        a->x1 = a->x1 & (~0x1); /* Ensure that x1 is even */
        a->x2 = a->x2 | 0x1;    /* Ensure that x2 is odd */
    }

    ...

    lv_display_add_event_cb(disp, rounder_event_cb, LV_EVENT_INVALIDATE_AREA, NULL);


Tiled Rendering
---------------

When multiple CPU cores are available and a large area needs to be redrawn, LVGL must identify independent areas that can be rendered in parallel.

For example, if there are 4 CPU cores, one core can draw the screen's background while the other 3 must wait until it is finished. If there are 2 buttons on the screen, those 2 buttons can be rendered in parallel, but 2 cores will still remain idle.

Due to dependencies among different areas, CPU cores cannot always be fully utilized.

To address this, LVGL can divide large areas that need to be updated into smaller tiles. These tiles are independent, making it easier to find areas that can be rendered concurrently.

Specifically, if there are 4 tiles and 4 cores, there will always be an independent area for each core within one of the tiles.

The maximum number of tiles can be set using the function :cpp:func:`lv_display_set_tile_cnt(disp, cnt)`. The default value is :cpp:expr:`LV_DRAW_SW_DRAW_UNIT_CNT` (or 1 if software rendering is not enabled).

Small areas are not further divided into smaller tiles because the overhead of spinning up 4 cores would outweigh the benefits.

The ideal tile size is calculated as ``ideal_tile_size = draw_buf_size / tile_cnt``. For example, in :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT` mode on an 800x480 screen, the display buffer is 800x480 = 375k pixels. If there are 4 tiles, the ideal tile size is approximately 93k pixels. Based on this, core utilization is as follows:

- 30k pixels: 1 core
- 90k pixels: 1 core
- 95k pixels: 2 cores (above 93k pixels, 2 cores are used)
- 150k pixels: 2 cores
- 200k pixels: 3 cores (above 186k pixels, 3 cores are used)
- 300k pixels: 4 cores (above 279k pixels, 4 cores are used)
- 375k pixels: 4 cores

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT`, the screen-sized draw buffer is divided by the tile count to determine the ideal tile sizes. If smaller areas are refreshed, it may result in fewer cores being used.

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_FULL`, the maximum number of tiles is always created as the entire screen is refreshed.

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL`, the partial buffer is divided into tiles. For example, if the draw buffer is 1/10th the size of the screen and there are 2 tiles, then 1/20th + 1/20th of the screen area will be rendered at once.

Tiled rendering only affects the rendering process, and the ``flush_cb`` is called once for each invalidated area. Therefore, tiling is not visible from the flushing point of view.


User data
---------

With :cpp:expr:`lv_display_set_user_data(disp, p)` a pointer to a custom data can
be stored in display object.

Decoupling the display refresh timer
------------------------------------

Normally the dirty (a.k.a invalid) areas are checked and redrawn in
every :c:macro:`LV_DEF_REFR_PERIOD` milliseconds (set in ``lv_conf.h``).
However, in some cases you might need more control on when the display
refreshing happen, for example to synchronize rendering with VSYNC or
the TE signal.

You can do this in the following way:

.. code-block:: c

   /*Delete the original display refresh timer*/
   lv_display_delete_refr_timer(disp);

   /*Call this anywhere you want to refresh the dirty areas*/
   _lv_display_refr_timer(NULL);

If you have multiple displays call :cpp:expr:`lv_display_set_default(disp1)` to
select the display to refresh before :cpp:expr:`_lv_display_refr_timer(NULL)`.


.. note:: that :cpp:func:`lv_timer_handler` and :cpp:func:`_lv_display_refr_timer` cannot  run at the same time.


If the performance monitor is enabled, the value of :c:macro:`LV_DEF_REFR_PERIOD` needs to be set to be
consistent with the refresh period of the display to ensure that the statistical results are correct.


Force refreshing
----------------

Normally the invalidated areas (marked for redraw) are rendered in :cpp:func:`lv_timer_handler` in every
:c:macro:`LV_DEF_REFR_PERIOD` milliseconds. However, by using :cpp:expr:`lv_refr_now(display)` you can ask LVGL to
redraw the invalid areas immediately. The refreshing will happen in :cpp:func:`lv_refr_now` which might take
longer time.

The parameter of :cpp:func:`lv_refr_now` is a display to refresh. If ``NULL`` is set the default display will be updated.

Events
******

:cpp:expr:`lv_display_add_event_cb(disp, event_cb, LV_EVENT_..., user_data)` adds
an event handler to a display. The following events are sent:

- :cpp:enumerator:`LV_EVENT_INVALIDATE_AREA` An area is invalidated (marked for redraw).
  :cpp:expr:`lv_event_get_param(e)` returns a pointer to an :cpp:struct:`lv_area_t`
  variable with the coordinates of the area to be invalidated. The area can
  be freely modified if needed to adopt it the special requirement of the
  display. Usually needed with monochrome displays to invalidate ``N x 8``
  rows or columns at once.
- :cpp:enumerator:`LV_EVENT_REFR_REQUEST`: Sent when something happened that requires redraw.
- :cpp:enumerator:`LV_EVENT_REFR_START`: Sent when a refreshing cycle starts. Sent even if there is nothing to redraw.
- :cpp:enumerator:`LV_EVENT_REFR_READY`: Sent when refreshing is ready (after rendering and calling the ``flush_cb``). Sent even if no redraw happened.
- :cpp:enumerator:`LV_EVENT_RENDER_START`: Sent when rendering starts.
- :cpp:enumerator:`LV_EVENT_RENDER_READY`: Sent when rendering is ready (before calling the ``flush_cb``)
- :cpp:enumerator:`LV_EVENT_FLUSH_START`: Sent before the ``flush_cb`` is called.
- :cpp:enumerator:`LV_EVENT_FLUSH_READY`: Sent when the ``flush_cb`` returned.
- :cpp:enumerator:`LV_EVENT_RESOLUTION_CHANGED`: Sent when the resolution changes due
  to :cpp:func:`lv_display_set_resolution` or :cpp:func:`lv_display_set_rotation`.


Further reading
***************

-  `lv_port_disp_template.c <https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_disp_template.c>`__
   for a template for your own driver.
-  :ref:`Drawing <porting_draw>` to learn more about how rendering
   works in LVGL.
-  :ref:`display_features` to learn more about higher
   level display features.

API
***
