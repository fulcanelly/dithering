#include "headers.hpp"


using std::vector;
using std::cout, std::endl;
using std::optional;
using std::lock_guard, std::mutex;

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

template<class T>
struct sync_lifo_queue {
    
    vector<T> queue;
    mutex queue_mutex;
    int size = 0;

    T pop() {
        lock_guard<mutex> guard(queue_mutex);
        return queue[--size];
    }

    void push(const T& value) {
        size++;
        lock_guard<mutex> guard(queue_mutex);
        queue.push_back(value);
    }

    bool is_not_empty() {
        return size;
    }
};


void process_image_by_path(boost::filesystem::path path, std::function<sf::Image(sf::Image)> transform) {
    sf::Image img;  
    img.loadFromFile(path.string());

    auto out = fs::path("out") / (path.filename().string() + std::string(".png"));

    transform(img)
        .saveToFile(out.string());
}

int main() {


    check_directory("imgs");
    check_directory("out");

    vector<std::thread> workers;
    sync_lifo_queue<fs::path> paths;

    for (auto img_path : fs::directory_iterator("imgs")) {
        paths.push(img_path.path());
    }

    for (size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
        auto work = [&, i] {
            while (paths.is_not_empty()) {
                auto path = paths.pop();
                cout << path.string() << " from worker " << i << endl;
                process_image_by_path(path, dither_ordered);
            }
        }; 
        workers.push_back(
            std::move(std::thread { work })
        );
    }

    for (auto &v : workers) {
        v.join();
    }

    return 0;

}
