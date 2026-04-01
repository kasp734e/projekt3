import time
import board
import neopixel

pixel_pin = board.D18
num_pixels = 128 

pixel_order = neopixel.RGBW  
pixels = neopixel.NeoPixel(
    pixel_pin, num_pixels, pixel_order=pixel_order, auto_write=False
)

# Clear all pixels to start
pixels.fill((0, 0, 0))
pixels.show()



for i in range(num_pixels):
    pixels[i] = (255,0,0)
    pixels.show()
    print(i)
    time.sleep(0.5)
