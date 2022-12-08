#include <iostream>
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <string>
#include "Image.hpp"
#include "Denoiser.hpp"
#include "Histogram.hpp"
#include "parameters.hpp"


using namespace std;
int main(int argc, char **argv) {

    string mode = "denoise";

    string input_name;
    string output_name;

    if (argc == 1) {
        cout << "This program needs to be used with arguments, as follows:" << endl;
        // crop the directory name from the path
        string program_name = argv[0];
        size_t last_slash = program_name.find_last_of('/');
        program_name = program_name.substr(last_slash + 1);
        cout << "\t ./" << program_name << " [ARGUMENTS]" << endl;
        cout << "Arguments:" << endl;
        cout << "\t --mode <mode> [OPTIONAL] {'denoise', 'countour_extractor', 'histogram'} (default is 'denoise')" << endl;
        cout << "\t --input <input file> [REQUIRED]" << endl;
        cout << "\t --output <output file> [OPTIONAL] (default is same as input)" << endl;
        cout << "\nTo edit the parameters used in each mode, edit the 'parameters.json' file in the root of the project." << endl;
        return 0;
    }

    if (argc % 2 == 0) {
        cout << "Error: Invalid number of arguments. Run ./main for to see the help screen" << endl;
        return 1;
    }

    bool valid = false;
    for (int i = 1; i < argc; i += 2) {
        string arg = argv[i];
        if (arg == "--mode") {
            if (strcmp(argv[i + 1], "denoise") == 0 || strcmp(argv[i + 1], "contour_detector") == 0 || strcmp(argv[i + 1], "histogram") == 0) {
                mode = argv[i + 1];
            }
            else {
                cout << "Error: Invalid mode. Run ./main for to see the help screen" << endl;
                return 1;
            }
        } else if (arg == "--input") {
            input_name = argv[i + 1];
            valid = true;
        } else if (arg == "--output") {
            output_name = argv[i + 1];
        } else {
            cout << "Error: Invalid argument: " << arg << endl;
            return 1;
        }
    }

    if (!valid) {
        cout << "Error: No input file specified. Run ./main for to see the help screen" << endl;
        return 1;
    }

    Image image(input_name);

    if (output_name.empty()) {
        output_name = image.getPath();
        size_t before_last_slash = output_name.find_last_of('/', output_name.find_last_of('/') - 1);
        size_t last_slash = output_name.find_last_of('/');
        output_name = output_name.substr(0, before_last_slash + 1) + "output/" + output_name.substr(last_slash + 1);

        // Find the position of the last '.' character in the string
        size_t dot_pos = output_name.rfind('.');

        // Insert the '_[mode]' string before the last '.' character
        output_name.insert(dot_pos, "_"+mode);
    } else {
        if (!image.usedAbsolutePath()) {
            string image_path = image.getPath();
            size_t before_last_slash = image_path.find_last_of('/', image_path.find_last_of('/') - 1);
            output_name = image_path.substr(0, before_last_slash + 1) + "output/" + output_name;
        }
    }

    if (mode == "denoise") {
        // show information about what is being done
        cout << "Denoising image: " << input_name << endl;
        cout << "Parameters used for denoising are:" << endl;
        cout << "\tKernel size: " << DENOISER_KERNEL_SIZE << endl;
        cout << "\tSigma: " << DENOISER_SIGMA << endl;
        cout << "\tOutput file: " << output_name << endl;

        Denoiser denoiser(DENOISER_KERNEL_SIZE, DENOISER_SIGMA);
        Image denoised_image = denoiser.denoise(image, true);
        denoised_image.save(output_name);
    }

    if (mode == "histogram") {
        // show information about what is being done
        cout << "Creating histogram of image: " << input_name << endl;
        cout << "Parameters used for histogram are:" << endl;
        cout << "\tBins: " << HISTOGRAM_BINS << endl;
        cout << "\tMin: " << HISTOGRAM_MIN << endl;
        cout << "\tMax: " << HISTOGRAM_MAX << endl;
        cout << "\tLog scale: " << LOG_SCALE << endl;
        cout << "\tOutput file: " << output_name << endl;

        Histogram histogram(HISTOGRAM_BINS, HISTOGRAM_MIN, HISTOGRAM_MAX, LOG_SCALE);
        histogram.getHistogram(image, output_name);
    }

    return 0;
}