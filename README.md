# Projector-off-o-matic

Turn off stubborn Android projectors (and probably other devices!) with your Firestick (and probably other!) remote.

### TL;DR

Having an Ardiuno send `Keyboard.press(238)` over USB will tell many devices to power down. This includes the RaspberryPi I tested with, and, fortunately, my projector.

Required parts:
* [IR receiver](https://www.adafruit.com/product/157) - $1.95
* [Adafruit Trinket M0](https://www.adafruit.com/product/3500) - $8.95
* [USB A to Micro-USB](https://www.amazon.com/UCEC-USB-2-0-Adapter-Male/dp/B00TAM0MZW/ref=sr_1_2_sspa?crid=1VM1GMB24SS14&dib=eyJ2IjoiMSJ9.Mh8r4f8nZ0ArrpLB7zx54d_EjoMORzVEEq0pQXyJGZm3q0uQDxfR7g2m7FMylaJYmvVrIBykpKXVcMI0ykn5XYRm6yGUmwKcrvactzzxJmVQ6AcBzJGP2I9oV57NqKf8VHQZjLlh2SnmlIvDY1jZfF76K00EdBV4z8A5ub4A7BfBCIHmox00Yz1SvfI79N45k_KGVowUZb1ibizEzmDkyzwzBLynPK8fyDpOlOVLL-Y.Gh9niLRsPlhiRSspRyeY6u4XrbAQkig8-e5F5vxGiT0&dib_tag=se&keywords=micro+usb+to+usb&qid=1715981410&sprefix=micro+usb+to+u%2Caps%2C249&sr=8-2-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1) - $8.99 for 2.

![Adafruit Tinket with IR receiver attached, plugged into a projector.](https://github.com/akostibas/projector-off-o-matic/blob/main/images/03%20-%20trinket%20installed.jpeg?raw=true)

## Problem

My entertainment setup consistsf of a [Nebula Solar Projector](https://us.seenebula.com/products/solar-portable-d2131) (now discontinued) mounted to the ceiling with a Firestick 4K Max plugged into it. Audio comes from a Echo + sub. It generally works pretty well!

The problem is that I would like to control the entire setup solely with the Firestick remote control. It mostly works, except that the projector does not turn off when I hit the Power button. The Firestick tries to send HDMI CEC commands to accomplish this, and it worked on a past projector, but this one is stubborn.

## Figuring out how to power off the projector

### The setup
The Anker projector runs an older version of Android. I don't recall which one; it's old enough that it doesn't get updates any more. But it also has a USB port!

I started experimenting with a keyboard I found laying around that has a curious "Turn Off" button on it... So, I plugged it into the projector, pushed it, and the thing powered off! 🎉 So, clearly there is some magic that a keyboard can send via USB to the projector to get it to turn off. How can I send that magic?

Well, [some Arduinos can act as USB HID devices](https://www.arduino.cc/reference/en/language/functions/usb/keyboard/) (ie. keyboards). Can I figure out what the keyboard code is, and get it to send that command?

### Getting an Arduino to send the right code

The Arduino keyboard library has a function that seemed like what I needed: `Keyboard.press()`. It takes a `char` as input and sends that over USB to the host device. You can use this to get up to [all sorts of antics](https://projecthub.arduino.cc/danionescu/arduino-keyboard-exploit-demo-hid-and-prevention-edf6df), but I just wanted it to send _one key code_. What's the code for "Turn Off," though?

Figuring out what code the keyboard I had was sending was a pretty big pain. I couldn't find a Mac application that would dump raw keyboard codes. The built in [Keyboard Viewer](https://support.apple.com/guide/mac-help/use-the-keyboard-viewer-on-mac-mchlp1015/mac) didn't seem to help, as it didn't give any indication that it knew about my magic Turn Off button, nor did it give me any idea of what codes the buttons were sending.

I ended up setting up a RaspberryPi running Linux (RaspbianOS) so that I could use [a Python script](https://github.com/akostibas/projector-off-o-matic/blob/main/experiments/hid-dump.py) that I found floating on the internet. Given an input device, it dumps information about what it receives from it. This ultimately worked, but there were some gotchas.

#### Finding the right input device
They regular "101-key keyboard" keys would show up when I logged `/dev/input/event0`, but it didn't register the Turn Off button, or other system keys like Volume Up / Down. Turns out the keyboard registers a few devices, and the Turn Off button events came over `/dev/input/event2`.

#### Finding the right key code
The event codes that this Python script were sending me didn't match the codes that the Ardiuno's `Keyboard.press()` function used. That is, the value for "A" sent by `Keyboard.press()` doesn't match the value that shows up in the event viewer.

Ultimately I just brute-forced it, and programmed an Arduino to cycle through 1-255 and send that to `Keyboard.press()`, and watched what I saw in the event viewer. Eventually I found the event that matched the code from the other keyboard! Spoiler: it's `238`.

## Setting up the Arduino to intercept the remote control's IR signal

Now that the Arduino can turn off the projector, I needed to find a way to have that trigger when the Firestick remote's Power button was hit. Luckily, the newer Firestick remotes (or at least the one I have) have IR transmitters. I pointed my phone camera at it while pressing the Power button, and sure enough I saw the IR LED flashing. So it should be possible.

To get the Arduino triggering the Power Off command when it saw the remote's IR, I needed to do a couple things:
1. Figure out how to wire up the IR receiver to the chip
2. Figure out how to have the Arduino interpret the signals it gets from the IR receiver

### Wiring it up

This was fairly easy, as [Adafruit has examples](https://learn.adafruit.com/ir-sensor) on their web page. I'll spare you the details and show you a picture:
![Arduino and Adafruit Trinket on breadboards with IR receivers wired up](https://github.com/akostibas/projector-off-o-matic/blob/main/images/01%20-%20arduino%20and%20trinket%20prototypes.jpeg?raw=true)

Note that I used an old Arduino Micro for prototyping because I had it laying around. Ironically, the much more powerful Trinket is cheaper and smaller, so I used that in the final build. They are both able to run the exact same code.

### Interpreting the IR signal

Once the IR receiver was connected, I wrote [some Arduino code](https://github.com/akostibas/projector-off-o-matic/blob/main/arduino/see-ir-codes.ino) to tell me what patterns it was seeing. There are probably better/smarter ways, but what this code did was:
1. Every millisecond, look to see if the IR receiver pin had changed states (Low -> High, or High -> Low)
2. If it did, start incrementing a counter for as long as it's in that state
3. When IR pin state changed again, print out the number of "ticks" (~1ms cycles) that state lasted

In testing, there were dozens of state changes with each button press. I up making a spreadsheet of the output, and found that across many presses of the power button, this code would get inconsistent number of ticks in each state, as well as have inconsistent numbers of state changes in general! That is, it was pretty noisy, so I had it only output one of these state changes if it remained for more than 5 ticks. Here's some output over several clicks of the remote:

![A spreadsheet showing how many ticks the IR remote spent in each state of its Power Off signal](https://github.com/akostibas/projector-off-o-matic/blob/main/experiments/ir%20state%20ticks.png?raw=true)

The green rows are for a High (1) signal from the IR receiver, and the bold entries are for the ones that I thought were consistent enough to try to use in my decoder.

Ultimately I was able to make [a simple state machine](https://github.com/akostibas/projector-off-o-matic/blob/main/arduino/detect-power-off.ino) to encode this, and it actually worked.

## Bob's your uncle!

With the IR decoder in hand, it was now simple to connect it to the function to send the power off key over USB. I was rather amazed that it all worked as advertised when I plugged it in! 😇
