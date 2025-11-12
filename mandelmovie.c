/// 
//  mandel.c
//  Based on example code found here:
//  https://users.cs.fiu.edu/~cpoellab/teaching/cop4610_fall22/project3.html
//
//  Converted to use jpg instead of BMP and other minor changes
//  
///
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "jpegrw.h"
#include <sys/wait.h>   // wait()

// local routines
static int iteration_to_color( int i, int max );
static int iterations_at_point( double x, double y, int max );
static void compute_image( imgRawImage *img, double xmin, double xmax,
									double ymin, double ymax, int max );
static void show_help();


int main( int argc, char *argv[] )
{
	int num_children = 4; // default num of children that can run at once
	int frames = 50; // num of images to make
	double xcenter = 0.0; // mandelbrot x center
	double ycenter = 0.0; // mandelbrot y center
	double scale = 4.0; // starting zool level
	char c;


	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"n:x:y:s:h"))!=-1) {
		switch(c) 
		{
			case 'n':
				num_children = atof(optarg);
				break;
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atoi(optarg);
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

	for(int i = 0; i < frames; i++){
		// make sure we dont spawn too many children 
		 if (i >= num_children) {
            wait(NULL);  // wait for one child to finish before spawning another
        }
		// pid == 0 → you are in the child process.
		// pid > 0 → you are in the parent process.
		// pid < 0 → the fork failed.
		 pid_t pid = fork();

        if (pid == 0) {
            // --- Child process ---
            double frame_scale = scale * (1.0 - (i * 0.02)); // zoom in a little each time
			// creates a unique filename for each image
            char filename[64];
			// naming format is 2 deceimal numbers 
			// sprintf prints to filename array
			// filename = "frame_00.jpg"
            sprintf(filename, "frame_%02d.jpg", i);

			// builds the command string to run the program
            char command[256];
            sprintf(command, "./mandel -x %.6f -y %.6f -s %.6f -o %s",
                    xcenter, ycenter, frame_scale, filename);

            printf("[Child %d] Generating %s (scale = %.6f)\n", getpid(), filename, frame_scale);

            system(command); // run command as typed into terminal
            exit(0);
        }
        else if (pid < 0) {
            perror("fork failed");
            exit(1);
        }
	}

	while (wait(NULL) > 0);

    printf("All frames generated successfully.\n");

	return 0;
}

// Show help message
void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-n <max>    The maximum number of children that can run at once. (default = 4)\n");
	printf("-x <coord>  X coordinate of image center point. (default = 0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default = 0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates (X-axis). (default=4)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}
