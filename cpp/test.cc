#include <SFML/Graphics/Image.hpp>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <vector>
#include <cmath>

#include <optional>

using std::vector;
using std::cout, std::endl;
using std::optional;

namespace fs = boost::filesystem;


const vector<int> mask = {
    0,  8,  2, 10,
    12, 4, 14,  6,
    3, 11,  1,  9,
    15, 7, 13,  5
} ;


inline sf::Uint8 apply_mask(int x, int y, sf::Uint8 color) {
    auto side = (sf::Uint8) sqrt(mask.size());
    if (color > mask.size() * mask[side * (x % side) + (y % side)]) {
        return 255;
    } else {
        return 0;

    };
}

sf::Image dither_ordered(sf::Image img) {
    sf::Image res;
    auto const size = img.getSize();
    res.create(size.x, size.y, sf::Color {0, 0, 0, 255});

    for (size_t i = 0; i < size.x; i++) {
        for (size_t j = 0; j < size.y; j++) {
            auto color = img.getPixel(i, j);  
            auto resc = sf::Color {
                apply_mask(i, j, color.r), apply_mask(i, j, color.g), apply_mask(i, j, color.b), 255
            };
            res.setPixel(i, j, resc);
        }
        
    }
    return res;
}


bool check_directory(std::string dir) {
    if (not fs::exists(dir)) {
        fs::create_directory(dir);
    }

    if (not fs::is_directory(dir)) {
        std::cerr << std::quoted(dir) << " should be directory";
        return true;
    }
    return false;
}

int main() {
    check_directory("imgs");
    check_directory("out");

    for (auto img_path : fs::directory_iterator("imgs")) {
        sf::Image img;  

        auto path = img_path.path();

        cout << path.string() << endl;

        img.loadFromFile(path.string());

        auto out = fs::path("out") / path.filename();

        dither_ordered(img)
            .saveToFile(out.string());
    
    }

    return 0;

}
