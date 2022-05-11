I changed the main function in order to select between a 3x3 or 4x4 Dithering matrix:
use either the command "test1.pgm result.pgm Dithering 4" or "test1.pgm result.pgm Dithering 4"
otherwise the default matrix is the 4x4 if you run "test1.pgm result.pgm Dithering"

I also added an extra parameter for the thresholding method,
One can call "test1.pgm result.pgm Thresholding noise" to use the Thresholding method with the noise.
(same for error diffusion)

For the Hilbert Curve Implementation I reused part of the code found at "https://en.wikipedia.org/wiki/Hilbert_curve".

For the bouns part use:
"./a.out test1.pgm result.pgm ErrorDiffusionExtension"
"./a.out test1.pgm result.pgm ErrorDiffusionExtension Hilbert"
"./a.out test1.pgm result.pgm ErrorDiffusionExtension combo"
