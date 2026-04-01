import board
import neopixel

line1 = None
line2 = None
line3 = None
line4 = None
line5 = None
line6 = None
line7 = None
line8 = None
line9 = None
line10 = None
line11 = None
line12 = None
line13 = None
line14 = None
line15 = None
line16 = None

pixles = None


def initLeds():
    global line1
    global line2
    global line3
    global line4
    global line5
    global line6
    global line7
    global line8
    global line9
    global line10
    global line11
    global line12
    global line13
    global line14
    global line15
    global line16
    global pixels

    line1 = [0, 1, 2, 3, 4, 5, 6, 7]
    line2 = [8, 9, 10, 11, 12, 13, 14, 15]
    line3 = [16, 17, 18, 19, 20, 21, 22, 23]
    line4 = [24, 25, 26, 27, 28, 29, 30, 31]
    line5 = [32, 33, 34, 35, 36, 37, 38, 39]
    line6 = [40, 41, 42, 43, 44, 45, 46, 47]
    line7 = [48, 49, 50, 51, 52, 53, 54, 55]
    line8 = [56, 57, 58, 59, 60, 61, 62, 63]
    line9 = [64, 65, 66, 67, 68, 69, 70, 71]
    line10 = [72, 73, 74, 75, 76, 77, 78, 79]
    line11 = [80, 81, 82, 83, 84, 85, 86, 87]
    line12 = [88, 89, 90, 91, 92, 93, 94, 95]
    line13 = [96, 97, 98, 99, 100, 101, 102, 103]
    line14 = [104, 105, 106, 107, 108, 109, 110, 111]
    line15 = [112, 113, 114, 115, 116, 117, 118, 119]
    line16 = [120, 121, 122, 123, 124, 125, 126, 127]

    pixel_pin = board.D18
    num_pixels = 128

    pixel_order = neopixel.RGBW
    pixels = neopixel.NeoPixel(
        pixel_pin, num_pixels, pixel_order=pixel_order, auto_write=False
    )


def clearPixels():
    global pixels
    pixels.fill((0, 0, 0))
    pixels.show()

def showPixels():
    global pixels
    pixels.show()


def setPixelsInLine(amountPixels, line):
    global line1
    global line2
    global line3
    global line4
    global line5
    global line6
    global line7
    global line8
    global line9
    global line10
    global line11
    global line12
    global line13
    global line14
    global line15
    global line16
    global pixels

    match line:
        case 1:
            for pixel in range(amountPixels):
                pixels[line1[pixel]] = (0, 255, 0)
        case 2:
            for pixel in range(amountPixels):
                pixels[line2[pixel]] = (16, 255, 0)
        case 3:
            for pixel in range(amountPixels):
                pixels[line3[pixel]] = (64, 255, 0)
        case 4:
            for pixel in range(amountPixels):
                pixels[line4[pixel]] = (128, 255, 0)
        case 5:
            for pixel in range(amountPixels):
                pixels[line5[pixel]] = (200, 255, 0)
        case 6:
            for pixel in range(amountPixels):
                pixels[line6[pixel]] = (255, 223, 0)
        case 7:
            for pixel in range(amountPixels):
                pixels[line7[pixel]] = (255, 191, 0)
        case 8:
            for pixel in range(amountPixels):
                pixels[line8[pixel]] = (255, 159, 0)
        case 9:
            for pixel in range(amountPixels):
                pixels[line9[pixel]] = (255, 127, 0)
        case 10:
            for pixel in range(amountPixels):
                pixels[line10[pixel]] = (255, 95, 0)
        case 11:
            for pixel in range(amountPixels):
                pixels[line11[pixel]] = (255, 63, 0)
        case 12:
            for pixel in range(amountPixels):
                pixels[line12[pixel]] = (160, 0, 255)
        case 13:
            for pixel in range(amountPixels):
                pixels[line13[pixel]] = (96, 0, 255)
        case 14:
            for pixel in range(amountPixels):
                pixels[line14[pixel]] = (48, 0, 255)
        case 15:
            for pixel in range(amountPixels):
                pixels[line15[pixel]] = (0, 16, 255)
        case 16:
            for pixel in range(amountPixels):
                pixels[line16[pixel]] = (0, 112, 255)
