# menusystem

An arduino library for implementing a menu system.

## Documentation

Other than the examples included in the library, the following pages might be
useful to you:

* [Arduino forum post](http://arduino.cc/forum/index.php/topic,105866.0.html)
* [Arduino Menu System Library](http://www.jonblack.me/arduino-menu-system-library/)

## Contribution

If you'd like to contribute to `arduino-menusystem`, please submit a
pull-request.

## Donate

* Bitcoin:  1D4MBvrR9w4yVEHvQbzM8WAuJjDdzp7trA
* Ethereum: 0x75804bb16c8177D919ea54a35980c9202F294A10

## Changelog

**3.0.0 - 24-08-2017**

* Factor out rendering a menu from its implementation
* Add `NumericMenuItem` and `BackMenuItem`
* Add `Menu` callback method
* `MenuSystem` creates its own root menu
* Hide non-api methods by making them private
* Deprecate `Menu::get_selected`

**2.1.1 - 22/03/2016**

* Fix regression - add reset option to select

**2.1.0 - 20/03/2016**

* Fix bug resetting the menu. Now possible via `reset()`

**2.0.2 - 23/01/2016**

* Fix deprecated conversion from string constant to char*
* Add Sure Electronics animated LED matrix example

**2.0.1 - 16/08/2015**

* Add Sure Electronics LED Matrix example

**2.0.0 - 14/08/2015**

* Add LCD Nav Keypad example from (thanks to @SauleVire)
* Remove AUTHORS file: too much hassle to maintain
* Add library.properties

**1.0.1 - 20/13/2013**

* Renamed project to `arduino-menusystem`;
* Fix buffer overflow when adding a menu;
* Menu components are now dynamically allocated (previously it was fixed at
  five items).

**1.0.0 - 15/12/2013**

* Selected menu item can be remembered (thanks to @KipK)
* Examples for serial and LCDs (thanks to @niesteszeck)
* Add AUTHORS file

**0.3.0 - 23/03/2013**

* Merged in another minor improvement provided by @thomasjfox:
 * Add the ability to loop through a menu in `prev()` and `next()`

**0.2.0 - 18/03/2013**

* Merged in minor improvements provided by thomasjfox
 * `get_menu_component()`: Use 'byte' instead of 16bit integer for index
 * Add return value to `MenuSystem.back()`
 * Prevent memory corruption by inserting too many items
 * Whitespace cleanup

**0.1.0 - 25/05/2012**

* Added support for displaying all current menu items (thanks to arcachofo)
* Added this readme

Note: The changes made for this release break backwards compatability. The
issues are relatively easy to fix.
