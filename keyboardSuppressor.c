#include <ApplicationServices/ApplicationServices.h>

// Note: The compiler will need to be told to use the apllication services framework. The command is:
// gcc -o keyboardSuppressor keyboardSuppressor.c -framework ApplicationServices

bool verbose = false;

int keyboard = 0;

int EXTERNAL_KEYBOARD = 44; // my external (USB) keyboard has a code of 44, my integrated (laptop) keyboard has a code of 92.
int INTEGRATED_KEYBOARD = 92; // Note that this might change on every boot, so this might need to be corrected on every boot.
// It seems to be resistant to unplugging and replugging though.

// Also note: it doesn't work on the lockscreen.


// Goal: Because my external keyboard has the quirk that it sends 9s whenever it wants to, 
// I want to intercept all keypresses and check if the keyboard is my external keyboard.
// If that keypress is also a 9 (or a ")" or "}" or "·", they all are pressed with the same key),
// I want to ignore it. Otherwise, I want to send it to the system as if it was a normal keypress.

CGEventRef
myCGEventCallback(CGEventTapProxy proxy, CGEventType type,
                  CGEventRef event, void *refcon)
{
    // Paranoid sanity check.
    if ((type != kCGEventKeyDown) && (type != kCGEventKeyUp))
        return event;

    keyboard = CGEventGetIntegerValueField(event, kCGKeyboardEventKeyboardType);
    CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

    // if you found your keyboard-value...
    // if (keyboard != MY_DEBUGGED_KEYBOARD)
    // {
    //     return event;
    // }

    // ... you can proceed with your stuff... i.e. remap input, etc.

    // Debugging time!

    if (verbose){
        printf("Gotten Keyboard: %d ", keyboard);
        printf("Gotten Keycode: %d\n", keycode);
    }

    // Debugging gave the info:
    // All keyboard inputs that are via the 9 key are of keycode 25.

    // So, if the event is a keydown event, and the keycode is 25, AND the keyboard is the external one,
    // we want to ignore it.

    if (type == kCGEventKeyDown && keycode == 25 && keyboard == EXTERNAL_KEYBOARD)
    {
        if (verbose) {
            printf("Ignoring keypress\n");
        }
        // Set the modified keycode field in the event.

        // Testing: set it to 1, if it would be a 9
        // keycode = (CGKeyCode)18;
        // CGEventSetIntegerValueField(event, kCGKeyboardEventKeycode, (int64_t)keycode); // Not a very good idea, but lets try it. (Backups are made)
        // Yay that worked! Now, we have to EAT the input, so that it doesn't get sent to the system.

        // Maybe returning NULL will do the trick?
        return NULL;
        // Yay! That worked! And as planned, the integrated keyboard is not affected by this.
        // The keyup event is irrelevant, we can ignore it. (I think)
        // Also notice that the keyboard IDs may change, so this might need to be corrected on every boot.

    }


    // We must return the event for it to be useful.
    return event;
}

int main(int argc, char *argv[])
{
    CFMachPortRef eventTap;
    CGEventMask eventMask;
    CFRunLoopSourceRef runLoopSource;

    // Search for an argument that is "-v" or "--verbose"
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
        {
            verbose = true;
        }
    }

    // Search for an argument that is parsable to an int
    for (int i = 0; i < argc; i++)
    {
        uintmax_t num = strtoumax(argv[i], NULL, 10);
        if (num != 0 && num != UINTMAX_MAX)
        {
            EXTERNAL_KEYBOARD = num;
            printf("Using keyboard ID: %d\n", EXTERNAL_KEYBOARD);
            break;
        }
    } 
    if (EXTERNAL_KEYBOARD == 44) {
        printf("Using default keyboard ID: %d\n", EXTERNAL_KEYBOARD);
    }

    // Create an event tap. We are interested in key presses.
    eventMask = ((1 << kCGEventKeyDown) | (1 << kCGEventKeyUp));
    eventTap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0,
                                eventMask, myCGEventCallback, NULL);
    if (!eventTap)
    {
        fprintf(stderr, "failed to create event tap\n");
        exit(1);
    }

    // Create a run loop source.
    runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);

    // Add to the current run loop.
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);

    // Enable the event tap.
    CGEventTapEnable(eventTap, true);

    // Set it all running.
    CFRunLoopRun();
    return 0;
}