# Projector-off-o-matic

Turn off stubborn Android projectors (and probably other devices!) with your Firestick (and probably other!) remote.

## Problem

My entertainment setup consistsf of a [Nebula Solar Projector](https://us.seenebula.com/products/solar-portable-d2131) (now discontinued) mounted to the ceiling with a Firestick 4K Max plugged into it. Audio comes from a Echo + sub. It generally works pretty well!

The problem is that I would like to control the entire setup solely with the Firestick remote control. It mostly works, except that the projector does not turn off when I hit the Power button. The Firestick tries to send HDMI CEC commands to accomplish this, and it worked on a past projector, but this one is stubborn.

## Figuring out how to power off the projector

The Anker projector runs an older version of Android. I don't recall which one, but old enough that it doesn't get updates any more :/ It also has a USB port! I started experimenting with a keyboard that has a curious "Turn Off" button on it... So, I pushed it, and the projector powered off! 🎉

So, clearly there is some magic that a keyboard can send via USB to the projector to get it to turn off. How can I send that magic?

Well, [some Arduinos can act as USB HID devices](https://www.arduino.cc/reference/en/language/functions/usb/keyboard/) (ie. keyboards). Can I figure out what the keyboard code is, and get it to send that command?

## Getting an Arduino to send the right code

The Arduino keyboard library has a function that seemed like what I needed: `Keyboard.press()`. It takes a `char` as input and sends that over USB to the host device. You can use this to get up to [all sorts of antics](https://projecthub.arduino.cc/danionescu/arduino-keyboard-exploit-demo-hid-and-prevention-edf6df), but I just wanted it to send _one key code_. What's the code for "Turn Off," though?

Figuring out what code the keyboard I had was sending was a pretty big pain. I couldn't find a Mac application that would dump raw keyboard codes. The built in [Keyboard Viewer](https://support.apple.com/guide/mac-help/use-the-keyboard-viewer-on-mac-mchlp1015/mac) didn't seem to help, as it didn't give any indication that it knew about my magic Turn Off button, nor did it give me any idea of what codes the buttons were sending.

<WIP>
