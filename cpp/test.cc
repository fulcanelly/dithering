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

int get_error(int chosen, int prev) {
    return chosen - prev;
}

int get_closest(int value) {
    if (value >= 127) { 
        return 255; 
    }
    else { 
        return 0; 
    }
}

int truncate(int val) {
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
using matrix =  boost::numeric::ublas::matrix<T>;

template<class T>
void add_checking_bounds(matrix<T> &arr, const T &val, const sf::Vector2i &pos, const sf::Vector2u &bounds) {
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


struct _s {}; //skip
struct _c {}; //center
struct nr {}; //next row

class diffusion_helper final {

private:
    
    std::vector<std::pair<double, sf::Vector2i>> factor_with_shift;
    sf::Vector2i pos = {0, 0};
    sf::Vector2i center = {0, 0};

public:

    diffusion_helper() {}

    diffusion_helper& next_x() {
        pos.x++;
        return *this;
    }

     diffusion_helper& operator<<(double x) {
        factor_with_shift.push_back({x, pos});
        return next_x();
    }
    
    diffusion_helper& operator<<(_s x) {
        return next_x();
    }

    diffusion_helper& operator<<(_c x) {
        center = pos;
        return next_x();
    }

    diffusion_helper& operator<<(nr x) {
        pos.y++;
        return *this;
    }

    diffusion_helper& setup_center() {
        for (auto &[_, pos] : factor_with_shift) {
            pos.x -= center.x;
            pos.y -= center.y;
        }
        return *this;
    } 

    diffusion_helper& normalize() {
        double sum = 0;
        for (auto &[val, _] : factor_with_shift) {
            sum += val;
        }

        for (auto &[val, _] : factor_with_shift) {
            val /= sum;
        }

        return *this;
    }

    diffusion_helper& show() {
        double s = 0; 
        for (auto &[x, val] : factor_with_shift) {
            s+= x;
            cout << x << endl;
        }  
        cout << factor_with_shift.size() << "====" << s <<  endl;
        return *this;
    }

    std::vector<std::pair<double, sf::Vector2i>> get_pairs() {
        return factor_with_shift;
    }

};

#define A 1

auto inline diffusion_matrix = (diffusion_helper {}
#ifdef A
<< _s() << _s() << _c() << 7.0 << 5.0 << nr()
         << 3.0 << 5.0 << 7.0 << 5.0 << 3.0 << nr()
         << 1.0 << 3.0 << 5.0 << 3.0 << 1.0 << nr())
#else
 << _c() << 0.6 << nr() 
 << 0.5 << 1 << nr()) 
#endif
    
    .normalize()
    .setup_center()
    .show()
    .get_pairs();

sf::Image dither_diffusion(sf::Image& img) {
    sf::Image res;
    const auto size = img.getSize();
    res.create(size.x, size.y, sf::Color {0, 0, 0, 255});

   

    auto err = rgb {
        matrix<int> {size.x, size.y},
        matrix<int> {size.x, size.y},
        matrix<int> {size.x, size.y}
    };


    for (unsigned i = 0; i < size.x ; ++ i) {
        for (unsigned j = 0; j < size.y; ++ j) {
            err.r (i, j) = 0;
            err.g (i, j) = 0;
            err.b (i, j) = 0;

        }   
    }
    for (int i = 0; i < size.x; i++) {
        for (int j = 0; j < size.y; j++) {


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

            for (auto& [val, shift] : diffusion_matrix) {
                sf::Vector2i adjusted = {shift.x + i, shift.y + j};
                add_checking_bounds(err.r, (int)(error_r * val), adjusted, size);
                add_checking_bounds(err.g, (int)(error_g * val), adjusted, size);
                add_checking_bounds(err.b, (int)(error_b * val), adjusted, size);

            }
        
             
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
