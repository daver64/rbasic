// Minimal SDL variable test
import "blib/sdl2.bas";

print("Testing SDL with variables...");

if (sdl_init("Test", 400, 300) == 0) {
    print("SDL init failed");
    return;
}

var counter = 0;
while (counter < 3) {
    var time_val = sdl_get_ticks();
    var calc_val = time_val + 100;
    print("counter = " + str(counter) + ", time_val = " + str(time_val) + ", calc_val = " + str(calc_val));
    counter = counter + 1;
    sdl_delay(100);
}

sdl_cleanup();
print("Test completed.");