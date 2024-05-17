def approach2():
    from evdev import InputDevice, categorize, ecodes

    # Find the keyboard device
    device = InputDevice('/dev/input/event0')  # Replace X with the actual event number

    # Print device info
    print(device)

    # Grab and print the events
    for event in device.read_loop():
        if event.type == ecodes.EV_KEY:
            key_event = categorize(event)
            if key_event.keystate == key_event.key_down:
                print(f"Key pressed: {key_event.keycode}")
