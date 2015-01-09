from PIL import Image

im = Image.open("/home/jaska/Development/u8glib/sys/sdl/helloworld/icons/original/battery-empty-2x.png")
newim = Image.new("P", im.size, 255)
newim.save("eka.bmp", "bmp")
newim.paste(im, mask=im.split()[3])
newim.save("testi.bmp", "bmp")
