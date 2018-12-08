# Triangled-Images
An evolutionary Hill Climbing program that approximates an image through a combination of triangles.

The program approximates a given image by using a fixed number of triangles (fixed before compilation)
and successively mutating either their colour or the position of their vertices, and keeping the mutation
if this improves the ''fitness''. The program works fast
even for large images. The secret to this lies in having small triangles and working locally. To ensure
the first, the initial population is made up of degenerate rectangles (see the example below) and the
mutatons are most-likely small. By working by locality, I mean that the fitness function need only
be evaluated locally (the points of the triangle). 

The inspiration for this program was the work [here](http://rogeralsing.com/2008/12/07/genetic-programming-evolution-of-mona-lisa/), in which genetic programming is used to approximate an image by using polygons.  

## Example

I consider a photograph of mine

![Original image](Example/examples1.png?style=centerme)

and I will run "./HCT examples1.png".


On the first generation we have as background the average colour of the image, and the triangles
are all degenerated to a point:

![First generation](Example/out0.png?style=centerme)

Result afater 100000 mutations (with 5000 triangles):

![After 100000 mutations](Example/out100000.png?style=centerme)




## Disclamer
This is a work from 2011. Not my most complete 2011 version, but at least the one I could still find on my pc. Probably going to be updated soon.
