#include <iostream>
#include <opencv2/opencv.hpp>
#include <cxxopts.hpp>

struct Config {
    const std::string file_path;
    const std::pair<int, int> top_left;
    const std::pair<int, int> lower_right;
};

Config parse_cli(int argc, char* argv[]) {
    cxxopts::Options options("regions", "Creates a negative section inside an image. Image will be resized to 200 x 200");

    options
        .positional_help("filepath")
        .show_positional_help();

    options.add_options()
        ("f,file", "path to the image file which will be transformed", cxxopts::value<std::string>())
        ("tlx", "horizontal position of top left corner", cxxopts::value<int>()->default_value("0"))
        ("tly", "vertical position of top left corner", cxxopts::value<int>()->default_value("0"))
        ("lrx", "horizontal position of lower right corner", cxxopts::value<int>()->default_value("200"))
        ("lry", "vertical position of lower right corner", cxxopts::value<int>()->default_value("200"))
        ("h,help", "Print usage")
    ;

    try {
        // Parsing arguments
        options.parse_positional({"file"});
        auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            exit(0);
        }

        auto file_path = result["file"].as<std::string>();
        auto top_left = std::make_pair(result["tlx"].as<int>(), result["tly"].as<int>());
        auto lower_right = std::make_pair(result["lrx"].as<int>(), result["lry"].as<int>());

        // Check if values are inside the acceptable range
        if (top_left.first < 0 || top_left.second < 0) {
            std::cout << "error parsing options: tlx or tly are out of range [0, 200]" << std::endl;
            exit(1);
        }

        if (lower_right.first > 200 || lower_right.second > 200) {
            std::cout << "error parsing options: lrx or lry are out of range [0, 200]" << std::endl;
            exit(1);
        }

        if (top_left.first > lower_right.first || top_left.second > lower_right.second) {
            std::cout << "error parsing options: top left values are greater than lower_right" << std::endl;
            exit(1);
        }

        return Config { file_path, top_left, lower_right };
    } catch (const cxxopts::OptionException& e) {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(1);
    }
}

/*
 * ROI and broadcasting.
 */
int main(int argc, char* argv[]) {
    auto config = parse_cli(argc, argv);

    cv::Mat image = cv::imread(config.file_path, cv::IMREAD_GRAYSCALE);

    // Resizing the image to 200x200
    cv::Mat small_image;
    cv::Size size { 200, 200 };
    cv::resize(image, small_image, size);

    // Renaming because Point2i has better attribute names than std::pair
    cv::Point2i top_left { config.top_left.first, config.top_left.second };
    cv::Point2i lower_right { config.lower_right.first, config.lower_right.second };

    // A better way using region of interest 
    int height = lower_right.x - top_left.x;
    int width = lower_right.y - top_left.y;
    cv::Rect roi { top_left.x, top_left.y, width, height };

    // Obtains a view of the section in which we're going to make the operation;
    cv::Mat image_roi = small_image(roi);
    // Applies a broadcasting operation
    image_roi = 255 - image_roi;

    cv::imshow("negative", small_image);
    cv::waitKey();
}
