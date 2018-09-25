# clipbored

clipbored is a distributed system for sharing and updating a large image
called a Bored.

A Bored is composed of Pastes which can be generated and submitted by anyone.
Pastes have a timestamp, a rectangular bitmap, and a target location on
the Bored.  Every Paste also has a hashcash value, which is computed as the
difficulty of its hash divided by 2 to the power of its age.  This total
hashcash value is divided by the number of pixels in the bitmap to produce a
per-pixel hashcash value for the Paste.  The Bored displays the highest-value
Paste in each pixel location.

When a Paste is completely covered, it can be forgotten.  So when in
equilibrium the storage required by the Bored is bounded by a constant.

Every Bored has a configurable half-life that determines the rate of
exponential decay of the hashcash value of its Pastes.

The tools:
        pastein
        pasteout
        pasteup
        pastedown
        genpaste
        runbored
        newbored

How to setup a Clipbored mirroring client:
        Run "newbored" with the parameters of the Bored you wish to mirror.
        It will create a directory called "testbored" and populate it with
        some files:

        $ ./newbored testbored testbored 1024 1024 86400
        
        Start "runbored".  It will render the Bored as a PNG file
        every 10 seconds so it can be viewed in a browser.

        $ nohup ./runbored testbored > runbored.out 2>&1 &

        There should be an index.html file in the testbored directory,
        navigate your browser there to take a look.  You should see two
        1024x1024 black images (the Bored and the heatmap).

        Now to listen for new pastes from another Bored, run pastedown:
        $ ./pastedown clipbored.io:4444 testbored

        You can also synchronize the entire Bored:
        $ ./pastedown clipbored.io:4443 testbored

        As Pastes arrive you should see the image update in your browser.

How to generate a Paste:

        First convert your image to a PPM file.  Now run:
        $ ./genpaste testbored 123 456 100 < image.ppm > image.paste

        This generates a paste at Bored position (123,456) with per-pixel
        hashcash value at least 100.  Then to apply the paste to a local
        Bored instance, run:
        $ ./pastein testbored < image.paste

        To apply it directly to a remote instance, run:
        $ nc clipbored.io 5555 < image.paste

        To uplink your local instance to a remote one, run:
        $ ./pasteup clipbored.io:5555 testbored
        This will watch for new pastes applied locally and send them to the
        remote instance.
        
How to garbage-collect an instance:

        $ ./newbored testbored.new testbored 1024 1024 86400
        $ ./recompress testbored |./pastein testbored.new
        $ mv testbored testbored.old
        $ mv testbored.new testbored
