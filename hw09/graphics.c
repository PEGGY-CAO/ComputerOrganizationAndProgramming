/**
* @file graphics.c
* @author Kaijun Lin
* @date 10/28/2018
* @brief A graphics library for drawing geometry, for Homework 9 of Georgia Tech
*        CS 2110, Fall 2018.
*/

// Please take a look at the header file below to understand what's required for
// each of the functions.
#include "graphics.h"

// Don't touch this. It's used for sorting for the drawFilledPolygon function.
int int_cmp(const void *a, const void *b)
{
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return *ia  - *ib;
}

Pixel noFilter(Pixel c) {
    // Don't touch this.
    return c;
}

// TODO: Complete according to the prototype in graphics.h
Pixel greyscaleFilter(Pixel c) {
    Pixel b = c;
    int mask = 0x1F;
    Pixel result = 0;
    int red = mask & b;
    int green = mask & (b >> 5);
    int blue = mask & (b >> 10);
    int n = (red * 77 + green * 151 + blue * 28) >> 8;
    result += n;
    result = result << 5;
    result += n;
    result = result << 5;
    result += n;
    c = result;
    return c;
}

// TODO: Complete according to the prototype in graphics.h
Pixel redOnlyFilter(Pixel c) {
    int mask = 0x1F;
    c = c & mask;
    return c;
}

// TODO: Complete according to the prototype in graphics.h
Pixel brighterFilter(Pixel c) {
    Pixel b = c;
    int mask = 0x1F;
    Pixel result = 0;
    int red = mask & b;
    int green = mask & (b >> 5);
    int blue = mask & (b >> 10);
    int max = mask;
    int diffRed = max - red;
    int newRed = red + (diffRed >> 1);
    int diffBlue = max - blue;
    int newBlue = blue + (diffBlue >> 1);
    int diffGreen = max - green;
    int newGreen = green + (diffGreen >> 1);
    result += newBlue;
    result = result << 5;
    result += newGreen;
    result = result << 5;
    result += newRed;
    c = result;
    return c;
}


// TODO: Complete according to the prototype in graphics.h
void drawPixel(Screen *screen, Vector coordinates, Pixel pixel) {
    if (coordinates.x < 0 || coordinates.x >= screen->size.x || coordinates.y < 0 || coordinates.y >= screen->size.y) {
        return;
    }
    int place = coordinates.y * screen->size.x + coordinates.x; 
    screen->buffer[place] = pixel;
}

// TODO: Comp   lete according to the prototype in graphics.h
void drawFilledRectangle(Screen *screen, Rectangle *rectangle) {
    int startX = rectangle->top_left.x;
    int startY = rectangle->top_left.y;
    int xCount = 0;
    while (xCount < rectangle->size.x) {
        int yCount = 0;
        while (yCount < rectangle->size.y) {
            drawPixel(screen, (Vector){xCount + startX, yCount + startY}, rectangle->color);
            yCount++;
        }   
        xCount++;
    }
    
}

// TODO: Complete according to the prototype in graphics.h
void drawLine(Screen *screen, Line *line) {
	int changed = 0;
    int startX = line->start.x;
    int startY = line->start.y;
    int endX = line->end.x;
    int endY = line->end.y;
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);
    int signx = 0;
    int signy = 0;
    if (endX - startX < 0) {
        signx = -1;
    } else if (endX - startX == 0) {
        signx = 0;
    } else {
        signx = 1;
    }
    if (endY - startY < 0) {
        signy = -1;
    } else if (endY - startY == 0) {
        signy = 0;
    } else {
        signy = 1;
    }
    if (dy > dx) {
        int temp = dx;
        dx = dy;
        dy = temp;
        changed = 1;
    }
    int e = 2 * dy - dx;
    for (int i = 1; i <= dx; i++) {
        drawPixel(screen, (Vector){startX, startY}, line->color);
        while (e >= 0) {
            if (changed) {
                startX += signx;
            } else {
                startY += signy;
            }
            e = e - 2 * dx;
        }
        if (changed) {
            startY += signy;
        } else {
            startX += signx;
        }
        e = e + 2 * dy;
    }
    drawPixel(screen, (Vector){endX, endY}, line->color);
}

// TODO: Complete according to the prototype in graphics.h
void drawPolygon(Screen *screen, Polygon *polygon) {
	int i = 0;
    int j = 1;
    while (i < polygon->num_vertices) {
        drawLine(screen, &(Line){polygon->vertices[i], polygon->vertices[j], polygon->color});
        i++;
        j = (j + 1) % polygon->num_vertices;
    }
}

// TODO: Complete according to the prototype in graphics.h
void drawFilledPolygon(Screen *screen, Polygon *polygon) {
	int min_y = INT_MAX;
    int max_y = INT_MIN;

    // -------------------------------------------------------------------------
    // TODO: Here, write some code that will find the minimum and maximum
    // Y values that belong to vertices of the polygon, and store them as
    // min_y and max_y.
    // -------------------------------------------------------------------------
    int p = 0;
    while (p < polygon->num_vertices) {
        if (polygon->vertices[p].y > max_y) {
            max_y = polygon->vertices[p].y;
        } 
        if (polygon->vertices[p].y < min_y) {
            min_y = polygon->vertices[p].y;
        }
        p++;
    }

    // -------------------------------------------------------------------------
    // END OF TODO
    // -------------------------------------------------------------------------

    // Now we iterate through the rows of our polygon
	for (int row = min_y; row <= max_y; row++) {
        // This variable contains the number of nodes found. We start with 0.
		int nNodes = 0;

        // This array will contain the X coords of the nodes we find.
        // Note that there are at most num_vertices of those. We allocate
        // that much room, but at any time only the first nNodes ints will
        // be our actual data.
        int nodeX[polygon->num_vertices];

        // This loop finds the nodes on this row. It intersects the line
        // segments between consecutive pairs of vertices with the horizontal
        // line corresponding to the row we're on. Don't worry about the
        // details, it just works.
		int j = polygon->num_vertices - 1;
		for (int i = 0; i < polygon->num_vertices; i++) {
			if ((polygon->vertices[i].y < row && polygon->vertices[j].y >= row) ||
				(polygon->vertices[j].y < row && polygon->vertices[i].y >= row)) {
				nodeX[nNodes++] = (polygon->vertices[i].x +
                    (row - polygon->vertices[i].y) *
                    (polygon->vertices[j].x - polygon->vertices[i].x) /
                    (polygon->vertices[j].y - polygon->vertices[i].y));
			}
			j = i;
		}

        // ---------------------------------------------------------------------
        // TODO: Make a call to qsort here to sort the nodeX array we made,
        // from small to large x coordinate. Note that there are nNodes elements
        // that we want to sort, and each is an integer. We give you int_cmp
        // at the top of the file to use as the comparator for the qsort call,
        // so you can just pass it to qsort and not need to write your own
        // comparator.
        // ---------------------------------------------------------------------
        qsort((void *)nodeX, nNodes, sizeof(int), int_cmp);

        // ---------------------------------------------------------------------
        // END OF TODO
        // ---------------------------------------------------------------------



        // ---------------------------------------------------------------------
        // TODO: Now we fill the x coordinates corresponding to the interior of
        // the polygon. Note that after every node we switch sides on the
        // polygon, that is, if we are on the outside, when we pass a node we
        // end up on the inside, and if are inside, we end up on the outside.
        // As a result, all you need to do is start at the 0th node, iterate
        // through all of the even-indexed nodes, and fill until the next node.
        // For example, you need to fill between nodes 0-1, between nodes 2-3,
        // nodes 4-5 etc. but not between nodes 1-2, or nodes 3-4.
        // ---------------------------------------------------------------------
        int k = 0;
        while (k < nNodes) {
            for (int s = nodeX[k]; s < nodeX[k + 1]; s++) {
                drawPixel(screen, (Vector){s, row}, polygon->color);
            }
            // drawLine(screen, &(Line){(Vector){nodeX[k], row}, (Vector){nodeX[k + 1] - 1, row}, polygon->color});
            k = k + 2;
        }
        // ---------------------------------------------------------------------
        // END OF TODO, 
        // ---------------------------------------------------------------------
	}
}

// TODO: Complete according to the prototype in graphics.h
void drawRectangle(Screen *screen, Rectangle *rectangle) {
    Vector vertices[4];
    Vector *v;
    v = vertices;
    *(v + 0) = rectangle->top_left;
    *(v + 1) = (Vector){rectangle->top_left.x + rectangle->size.x - 1, rectangle->top_left.y};
    *(v + 2) = (Vector){rectangle->top_left.x + rectangle->size.x - 1, rectangle->top_left.y + rectangle->size.y - 1};
    *(v + 3) = (Vector){rectangle->top_left.x, rectangle->top_left.y + rectangle->size.y - 1};
    drawPolygon(screen, &(Polygon){v, 4, rectangle->color});
}

// TODO: Complete according to the prototype in graphics.h
void drawCircle(Screen *screen, Circle *circle) {
    int x = 0;
    int y = circle->radius;
    int d = 1 - circle->radius;
    int mx = circle->center.x;
    int my = circle->center.y;
    while (x <= y) {
        drawPixel(screen, (Vector){mx + x, my + y}, circle->color);
        drawPixel(screen, (Vector){mx + x, my - y}, circle->color);
        drawPixel(screen, (Vector){mx - x, my + y}, circle->color);
        drawPixel(screen, (Vector){mx - x, my - y}, circle->color);
        drawPixel(screen, (Vector){mx + y, my + x}, circle->color);
        drawPixel(screen, (Vector){mx + y, my - x}, circle->color);
        drawPixel(screen, (Vector){mx - y, my + x}, circle->color);
        drawPixel(screen, (Vector){mx - y, my - x}, circle->color);
        if (d < 0) {
            d = d + 2 * x + 3;
            x += 1;
        } else {
            d = d + 2 * (x - y) + 5;
            x += 1;
            y -= 1;
        }
    }
}

// TODO: Complete according to the prototype in graphics.h
void drawFilledCircle(Screen *screen, Circle *circle) {
    int x = 0;
    int y = circle->radius;
    int d = 1 - circle->radius;
    int mx = circle->center.x;
    int my = circle->center.y;
    while (x <= y) {
        Vector p1 = (Vector){mx + x, my + y};
        Vector p2 = (Vector){mx + x, (my > y) ? (my - y) : 0};
        drawLine(screen, &(Line){p1, p2, circle->color});

        if (mx >= x) {
            Vector p3 = (Vector){mx - x, my + y};
            Vector p4 = (Vector){mx - x, (my > y) ? my - y : 0};
            drawLine(screen, &(Line){p3, p4, circle->color});
        }

        Vector p5 = (Vector){mx + y, my + x};
        Vector p6 = (Vector){mx + y, (my > x) ? (my - x) : 0};
        drawLine(screen, &(Line){p5, p6, circle->color});

        if (mx >= y) {
            Vector p7 = (Vector){mx - y, my + x};
            Vector p8 = (Vector){mx - y, (my > x) ? (my - x) : 0};
            drawLine(screen, &(Line){p7, p8, circle->color});
        }

        if (d < 0) {
            d = d + 2 * x + 3;
            x += 1;
        } else {
            d = d + 2 * (x - y) + 5;
            x += 1;
            y -= 1;
        }
    }
}

// TODO: Complete according to the prototype in graphics.h
void drawImage(Screen *screen, Image *image, Pixel (*colorFilter)(Pixel)) {
    int count = 0;
    int startX = image->top_left.x;
    int startY = image->top_left.y;
    int x = image->size.x;
    int y = image->size.y;
    int yCount = 0;
    while (yCount < y) {
        int xCount = 0;
        while (xCount < x) {
            drawPixel(screen, (Vector){xCount + startX, yCount + startY}, (*colorFilter)(image->buffer[count]));
            count++;
            xCount++;
        }   
        yCount++;
    }
}

// TODO: Complete according to the prototype in graphics.
Image rotateImage(Image *image, int degrees) {
    Pixel* copyBuffer = (Pixel*)malloc((image->size.x * image->size.y) * sizeof(Pixel));
    //Pixel* temp1 = copyBuffer;
    for (int i = 0; i < image->size.x * image->size.y; i++) {
        copyBuffer[i] = 0x7fff;
    }
    while (degrees < 0) {
        degrees += 360;
    }
    degrees = degrees % 360;
    if (degrees == 0) {
        //temp1 = copyBuffer;
        Pixel* temp2 = image->buffer;
        for (int i = 0; i < image->size.x * image->size.y; i++) {
	    copyBuffer[i] = *temp2;
            temp2++;
        }
        return (Image){(Vector){image->top_left.x, image->top_left.y}, (Vector){image->size.x, image->size.y}, copyBuffer};
    } else if (degrees == 180) {
        int i = image->size.x * image->size.y - 1;
        int j = 0;
        while (i >= 0) {
            copyBuffer[j] = image->buffer[i];
            i--;
            j++;
        }
        return (Image){(Vector){image->top_left.x, image->top_left.y}, (Vector){image->size.x, image->size.y}, copyBuffer};
    } else if (degrees == 90) {
        for (int i = 0; i < image->size.x; i++) {
            for (int j = 0; j < image->size.y; j++) {
                int original = j * image->size.x + i;
                int newY = image->size.x - 1 - i;
                int newPlace = newY * image->size.y + j;
                copyBuffer[newPlace] = image->buffer[original];
            }
        }
        return (Image){(Vector){image->top_left.x, image->top_left.y}, (Vector){image->size.y, image->size.x}, copyBuffer};
    } else {
        for (int i = 0; i < image->size.x; i++) {
            for (int j = 0; j < image->size.y; j++) {
                int place = j * image->size.x + i;
                int newX = image->size.y - j - 1;
                int newPlace = i * image->size.y + newX;
                copyBuffer[newPlace] = image->buffer[place];
            }
        }
        return (Image){(Vector){image->top_left.x, image->top_left.y}, (Vector){image->size.y, image->size.x}, copyBuffer};
    }
}
