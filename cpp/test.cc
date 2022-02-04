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

using x_t = size_t;
using y_t = size_t;

long long get_error(long long chosen, long long prev) {
    return chosen - prev;
}

long long get_closest(long long value) {
    if (value >= 127) { 
        return 255; 
    }
    else { 
        return 0; 
    }
}

long long truncate(long long val) {
    if (val < 0) {
        return 0;
    }

    if (val > 255) {
        return 255;
    }
        
    return val;
}


const auto diffusion_d = sf::Vector2 { 5, 3 };

template<class T>
void add_checking_bounds(boost::numeric::ublas::matrix<T> &arr, const T &val, const sf::Vector2u &pos, const sf::Vector2u &bounds) {
    if (pos.x >= bounds.x || pos.y >= bounds.y) {
        return;
    } else {
        arr(pos.x, pos.y) += val;
    }
} 

template<class T>
struct rgb {
    T r, g, b;
};

sf::Image dither_diffusion(sf::Image& img) {
    sf::Image res;
    const auto size = img.getSize();
    res.create(size.x, size.y, sf::Color {0, 0, 0, 255});

    using namespace boost::numeric::ublas;
   

    auto err = rgb {
        matrix<long long> {size.x, size.y},
        matrix<long long> {size.x, size.y},
        matrix<long long> {size.x, size.y}
    };


    for (unsigned i = 0; i < size.x ; ++ i) {
        for (unsigned j = 0; j < size.y; ++ j) {
            err.r (i, j) = 0;
            err.g (i, j) = 0;
            err.b (i, j) = 0;

        }   
    }
    for (uint32_t i = 0; i < size.x; i++) {
        for (uint32_t j = 0; j < size.y; j++) {


            auto pixel = img.getPixel(i, j);

            auto color_r = truncate(
                pixel.r - err.r (i, j));  
            
            auto color_g = truncate(
                pixel.g - err.g (i, j));  
            
            auto color_b = truncate(
                pixel.b -  err.b (i, j));  


            auto closest_r = get_closest(color_r);
            auto closest_g = get_closest(color_g);
            auto closest_b = get_closest(color_b);

            res.setPixel(i, j, sf::Color {
                static_cast<sf::Uint8>(closest_r), 
                static_cast<sf::Uint8>(closest_g), 
                static_cast<sf::Uint8>(closest_b),
                255
            });
        
            
            auto error_r = get_error(closest_r, color_r);
            auto error_g = get_error(closest_g, color_g);
            auto error_b = get_error(closest_b, color_b);
            if (false) { 
                add_checking_bounds(err.r, (long long)(error_r * 0.5), {i + 1, j    }, size);
                add_checking_bounds(err.r, (long long)(error_r * 1),   {i + 1, j + 1}, size);
                add_checking_bounds(err.r, (long long)(error_r * 0.5), {i    , j + 1}, size);



                add_checking_bounds(err.g, (long long)(error_g * 0.5), {i + 1, j}, size);
                add_checking_bounds(err.g, (long long)(error_g * 1), {i + 1, j + 1}, size);
                add_checking_bounds(err.g, (long long)(error_g * 0.5), {i, j + 1}, size);

                add_checking_bounds(err.b, (long long)(error_b * 0.5), {i + 1, j}, size);
                add_checking_bounds(err.b, (long long)(error_b * 1), {i + 1, j + 1}, size);
                add_checking_bounds(err.b, (long long)(error_b * 0.5), {i, j + 1}, size);
            }

            add_checking_bounds(err.r, (long long)(error_r * 7/16.0), {i + 1, j    }, size);
            add_checking_bounds(err.r, (long long)(error_r * 1/16.0),   {i + 1, j + 1}, size);
            add_checking_bounds(err.r, (long long)(error_r * 5/16.0), {i    , j + 1}, size);
            add_checking_bounds(err.r, (long long)(error_r * 3/16.0), {i - 1, j + 1}, size);
             

            add_checking_bounds(err.g, (long long)(error_g * 7/16.0), {i + 1, j    }, size);
            add_checking_bounds(err.g, (long long)(error_g * 1/16.0),   {i + 1, j + 1}, size);
            add_checking_bounds(err.g, (long long)(error_g * 5/16.0), {i    , j + 1}, size);
            add_checking_bounds(err.g, (long long)(error_g * 3/16.0), {i - 1, j + 1}, size);
             
            add_checking_bounds(err.b, (long long)(error_b * 7/16.0), {i + 1, j    }, size);
            add_checking_bounds(err.b, (long long)(error_b * 1/16.0),   {i + 1, j + 1}, size);
            add_checking_bounds(err.b, (long long)(error_b * 5/16.0), {i    , j + 1}, size);
            add_checking_bounds(err.b, (long long)(error_b * 3/16.0), {i - 1, j + 1}, size);
             
        }


    }

//
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


void process_image_by_path(boost::filesystem::path path, std::function<sf::Image(sf::Image&)> transform) {
    sf::Image img;  
    img.loadFromFile(path.string());

    auto out = fs::path("out") / (path.filename().string());

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
        auto work = [&paths, i] {
            while (paths.is_not_empty()) {
                auto path = paths.pop();
                cout << path.string() << " from worker " << i << endl;
                process_image_by_path(path, dither_diffusion);
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
