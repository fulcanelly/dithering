package me.fulcanelly.dither;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.function.Consumer;
import java.util.stream.Stream;

import javax.imageio.ImageIO;

import me.fulcanelly.dither.handlers.BayerDitheringFacade;
import me.fulcanelly.dither.handlers.ErrorDiffusionFacade;
import me.fulcanelly.dither.handlers.ImageHandler;
import me.fulcanelly.dither.utils.Traverser;

import java.awt.Color;
import java.awt.image.BufferedImage;

class Layer {
    Color avgColor;

    public static void main(String[] args) throws Exception {
        Main.main(args);
    }
}

class Position {
    public Position(int i, int j) {
        this.x = i;
        this.y = j;
    }

    @Override
    public String toString() {
        return "{" + x + ", " + y + "}";
    }

    int x, y;
}

class DownLayer extends Layer {

    List<Position> indexes;
}

class AnyLayer extends Layer {
    List<DownLayer> indexes;
}

interface PyramidNodeX {

    Color getAvgColor();

    Stream<Position> getPositionsStream();
}

class TopPyramidNode implements PyramidNodeX {

    List<Position> indexes;
    Color avgColor;

    public TopPyramidNode(Color avgColor, List<Position> indexes) {
        this.avgColor = avgColor;
        this.indexes = indexes;
    }

    @Override
    public Stream<Position> getPositionsStream() {
        return indexes.stream();
    }

    @Override
    public Color getAvgColor() {
        return avgColor;
    }

}

class BigPyramidNode implements PyramidNodeX {
    Color avgColor;
    List<Position> indexes;

    public BigPyramidNode(Color avgColor, List<Position> indexes) {
        this.avgColor = avgColor;
        this.indexes = indexes;
    }

    @Override
    public Stream<Position> getPositionsStream() {
        return indexes.stream();
    }

    @Override
    public Color getAvgColor() {
        return avgColor;
    }
}

class PyramidNode {
    Color avgColor;
    List<Position> indexes;

    public PyramidNode(Color avColor, List<Position> indexes) {
        this.avgColor = avColor;
        this.indexes = indexes;
    }
}

class PyramidLayer {
    final PyramidNodeX[][] nodes;

    final int width, height;

    public int getHeight() {
        return height;
    }

    public int getWidth() {
        return width;
    }

    public PyramidLayer(int width, int height) {
        this.height = height;
        this.width = width;
        this.nodes = new PyramidNodeX[width][height];
    }

    PyramidNodeX[][] nodes() {
        return nodes;
    }

    @Override
    public String toString() {
        return String.format("PyramidLayer !{w: %d, h: %d}", width, height);
    }
}

class FinalPyrmid {

}

// TODO, even now i see that constructNodeOfImage and constructNodeOfLayer can
// be the same method
// (should convert image to layer at start)
class PyramidSegmenter {

    public static double hsvColorDistance2(Color a, Color b) {
        var hsv1 = Color.RGBtoHSB(a.getRed(), a.getGreen(), a.getBlue(), null);
        var hsv2 = Color.RGBtoHSB(b.getRed(), b.getGreen(), b.getBlue(), null);

        double hue1 = hsv1[0], sat1 = hsv1[1], val1 = hsv1[2];
        double hue2 = hsv2[0], sat2 = hsv2[1], val2 = hsv2[2];

        // Calculate hue distance using the shortest distance around the color wheel
        double hueDistance = Math.min(Math.abs(hue1 - hue2), 1 - Math.abs(hue1 - hue2));

        // Calculate saturation and value distances
        double satDistance = sat1 - sat2;
        double valDistance = val1 - val2;

        // Calculate color distance using a weighted combination of the three distances
        double colorDistance = Math.sqrt(0.5 * hueDistance * hueDistance +
                0.4 * satDistance * satDistance +
                0.1 * valDistance * valDistance);

        return colorDistance;
    }

    double colorDistance(Color color1, Color color2) {
        return hsvColorDistance2(color1, color2);
    }

    boolean isColorsClose(Color a, Color b) {
        // var dist = hsvColorDistance2(color1, color2);;
        return hsvColorDistance2(a, b) <= 0.1;
    }

    // double colorDistance(Color color1, Color color2) {

    // return Math.sqrt(Math.pow(color1.getRed() - color2.getRed(), 2) +
    // Math.pow(color1.getGreen() - color2.getGreen(), 2) +
    // Math.pow(color1.getBlue() - color2.getBlue(), 2));
    // }

    // boolean isColorsClose(Color color1, Color color2) {
    // double distance = Math.sqrt(Math.pow(color1.getRed() - color2.getRed(), 2) +
    // Math.pow(color1.getGreen() - color2.getGreen(), 2) +
    // Math.pow(color1.getBlue() - color2.getBlue(), 2));

    // double threshold = 0.1 * Math.sqrt(255 * 255 * 3);
    // return distance <= threshold;
    // }

    /*
     *
     *
     * I'm trying to do is implement pyramidal image segmentation, sort of but...
     *
     * I can' figure out how to obtain indexes of most common pixels in neighbours
     * (this array size can be from 1 to 4)
     *
     * by common i mean that pixels which colors is most close to each other color
     * among neighbours
     *
     * for example if all neighbours colours are the same, then this array is
     * maximum it can get's: it contains index of all underlying pixels
     *
     * or opposite, if all pixels equally far away from each other - that that array
     * contains only one index of pixel – the one that color is closes to avg
     * color(among this chunk of to call that)
     *
     * and so on, two or three colors
     *
     * here is the method
     *
     *
     * you can use method double colorDistance(Color color1, Color color2)
     *
     * count of collor.. ? it will almost always 1 for each, you should look by
     * color distance: double colorDistance(Color color1, Color color2)
     */
    PyramidNode constructNodeOfImage2(BufferedImage image, int x, int y) {
        int width = image.getWidth();
        int height = image.getHeight();

        List<Position> indexes = new ArrayList<>();

        /// index calcaulation happens here
        for (int i = x; i <= x + 1 && i < width; i++) {
            for (int j = y; j <= y + 1 && j < height; j++) {
                // TODO
            }
        }

        Color avgColor = calculateAvgColor(
                indexes.stream()
                        .map(pos -> new Color(image.getRGB(pos.x, pos.y)))
                        .toList());

        return new PyramidNode(avgColor, indexes);
    }

    Position getPositionOfClosestTo(BufferedImage image, Color target, int x, int y) {
        Position closestPosition = null;

        double minDistance = Double.POSITIVE_INFINITY;

        for (int i = x; i <= x + 1 && i < image.getWidth(); i++) {
            for (int j = y; j <= y + 1 && j < image.getHeight(); j++) {
                Color pixelColor = new Color(image.getRGB(i, j));
                double distance = colorDistance(pixelColor, target);

                if (distance < minDistance) {
                    minDistance = distance;
                    closestPosition = new Position(i, j);
                }
            }
        }

        return closestPosition;
    }

    // returns first layer of image node(contains indexes of pizels that it
    // includes)
    // so it checks it's pixel, (x + 1, y), (x, y + 1), (x + 1, y +1) boundary check
    // needed also, skip pixels if they out
    // so node that returned should contain the most popular common (avg) color in
    // that range of pixels
    // and indexes of that pixels that close to that color and betwwen each other
    PyramidNodeX constructNodeOfImage(BufferedImage image, int x, int y) {
        int width = image.getWidth();
        int height = image.getHeight();

        List<Position> indexes = new ArrayList<>();

        Color baseColor = calculateAvgColor(getNeighborColors(image, x, y));

        for (int i = x; i <= x + 1 && i < width; i++) {
            for (int j = y; j <= y + 1 && j < height; j++) {
                Color color = new Color(image.getRGB(i, j));

                if (isColorsClose(baseColor, color)) {
                    indexes.add(new Position(i, j));
                }
            }
        }

        // TODO, i don't think it's good implementation
        if (indexes.isEmpty()) {
            indexes.add(getPositionOfClosestTo(image, baseColor, x, y));
        }

        Color avgColor = calculateAvgColor(
                indexes.stream()
                        .map(pos -> new Color(image.getRGB(pos.x, pos.y)))
                        .toList());

        return new TopPyramidNode(avgColor, indexes);
    }

    Stream<Position> getNeighbors(int x, int y) {
        return null;
    }

    // TODO, bounds, pos
    List<Position> nearbyPositionsStream(final Position bound, final Position position) {
        var list = new ArrayList<Position>();

        for (int i = position.x; i <= position.x + 1 && i < bound.x; i++) {
            for (int j = position.y; j <= position.y + 1 && j < bound.y; j++) {
                list.add(new Position(i, j));
            }
        }

        return list;
    }

    PyramidNodeX constructNodeOfLayer(PyramidLayer prevLayer, int x, int y) {
        final int width = prevLayer.getWidth();
        final int height = prevLayer.getHeight();

        var nearby = nearbyPositionsStream(new Position(width, height), new Position(x, y))
                .stream()
                .map(pos -> prevLayer.nodes()[pos.x][pos.y])
                .toList();

        var baseColor = calculateAvgColor(
                nearby.stream()
                        .map(PyramidNodeX::getAvgColor)
                        .toList());

        var childNodes = nearby.stream()
                .filter(node -> isColorsClose(node.getAvgColor(), baseColor))
                .toList();

        if (childNodes.isEmpty()) {
            childNodes = nearby.stream()
                    .sorted((a, b) -> Double.compare(
                            colorDistance(a.getAvgColor(), baseColor),
                            colorDistance(a.getAvgColor(), baseColor)))
                    .findFirst().stream().toList();

            // childNodes.add(getClosestByColorNearNode(prevLayer, baseColor, x, y));
        }

        // Color avgColor = calculateAvgColor(
        // indexes.stream()
        // .map(pos -> prevLayer.nodes[pos.x][pos.y].avgColor)
        // .toList());

        var avgColor = calculateAvgColor(childNodes.stream()
                .map(PyramidNodeX::getAvgColor).toList());

        var positions = childNodes.stream()
                .flatMap(i -> i.getPositionsStream())
                .toList();

        return new BigPyramidNode(avgColor, positions);// new
                                                       // PyramidNode(avgColor,
                                                       // indexes);

    }

    PyramidNodeX getClosestByColorNearNode(PyramidLayer layer, Color target, int x, int y) {
        PyramidNodeX closestNode = null;

        double minDistance = Double.POSITIVE_INFINITY;

        for (int i = x; i <= x + 1 && i < layer.getWidth(); i++) {
            for (int j = y; j <= y + 1 && j < layer.getHeight(); j++) {
                var node = layer.nodes()[i][j];
                var pixelColor = node.getAvgColor();
                var distance = colorDistance(pixelColor, target);

                if (distance < minDistance) {
                    minDistance = distance;
                    closestNode = node;
                }
            }
        }

        return closestNode;
    }

    public static List<Color> getNeighborColors(PyramidLayer image, int x, int y) {
        List<Color> neighborColors = new ArrayList<>();
        int width = image.getWidth();
        int height = image.getHeight();

        for (int i = x; i <= x + 1 && i < width; i++) {
            for (int j = y; j <= y + 1 && j < height; j++) {
                neighborColors.add(image.nodes()[i][j].getAvgColor());
            }
        }

        return neighborColors;
    }

    public static List<Color> getNeighborColors(BufferedImage image, int x, int y) {
        List<Color> neighborColors = new ArrayList<>();
        int width = image.getWidth();
        int height = image.getHeight();

        for (int i = x; i <= x + 1 && i < width; i++) {
            for (int j = y; j <= y + 1 && j < height; j++) {
                neighborColors.add(new Color(image.getRGB(i, j)));
            }
        }

        return neighborColors;
    }

    public static Color calculateAvgColor(List<Color> colors) {
        int totalColors = colors.size();
        int redSum = colors.stream().mapToInt(Color::getRed).sum();
        int greenSum = colors.stream().mapToInt(Color::getGreen).sum();
        int blueSum = colors.stream().mapToInt(Color::getBlue).sum();

        return new Color(
                redSum / totalColors,
                greenSum / totalColors,
                blueSum / totalColors);
    }

    BufferedImage applyPyramid(BufferedImage img, PyramidLayer layer) {
        var rand = ThreadLocalRandom.current();

        for (var nodes : layer.nodes) {
            for (var node : nodes) {

                // var color = node.avgColor;
                var color = new Color(rand.nextInt(250), rand.nextInt(250), rand.nextInt(250)); // .getRGB()

                for (var pos : node.getPositionsStream().toList()) {
                    img.setRGB(pos.x, pos.y, color.getRGB());

                    // image.setRGB(pos.x, pos.y, );
                }
            }
        }
        return img;
    }

    Color getRandColor() {
        var rand = ThreadLocalRandom.current();

        var color = new Color(rand.nextInt(250), rand.nextInt(250), rand.nextInt(250));
        return color;
    }

    void applyForEachPixelOfNode(BufferedImage img, PyramidNodeX node, Consumer<Position> apply) {
        node.getPositionsStream().forEach(apply);
    }

    void colorifyEachNode(BufferedImage img, PyramidLayer layer) {
        for (var nodes : layer.nodes()) {
            for (var node : nodes) {
                var color = node.getAvgColor();
                System.out.println(node.getPositionsStream().count());
                node.getPositionsStream().forEach((pos) -> img.setRGB(pos.x, pos.y, color.getRGB()));

            }
        }
    }

    BufferedImage handle(BufferedImage image) {

        System.out.println(String.format("Hui {w %d h %d}", image.getWidth(), image.getHeight()));
        final var firstLayer = new PyramidLayer(image.getWidth() / 2, image.getHeight() / 2);
        for (int i = 0; i < image.getWidth() / 2; i++) {
            for (int j = 0; j < image.getHeight() / 2; j++) {
                firstLayer.nodes()[i][j] = constructNodeOfImage(image, i * 2, j * 2);
            }
        }

        var currentLayer = firstLayer;

        while (Math.min(currentLayer.height, currentLayer.width) > 1) {
            var newLayer = new PyramidLayer(currentLayer.width / 2, currentLayer.height / 2);
            System.out.println(currentLayer);

            for (int i = 0; i < newLayer.getWidth(); i++) {
                for (int j = 0; j < newLayer.getHeight(); j++) {
                    newLayer.nodes()[i][j] = constructNodeOfLayer(currentLayer, i * 2, j * 2);
                }
            }

            currentLayer = newLayer;

            System.out.println(currentLayer);

        }

        // count child node coutn

        // System.out.println(currentLayer.nodes()[0][0].getPositionsStream().count());

        // highlight

        System.out.println(firstLayer);

        colorifyEachNode(image, currentLayer);
        return image;
    }
}

class HSL {
    float h, s, l;

    @Override
    public String toString() {
        return String.format("h: %f s: %f l: %f", h, s, l);
    }
}

class HueRgbConverter {

    double colorDistance(Color a, Color b) {
        var ahsv = Color.RGBtoHSB(a.getRed(), a.getGreen(), a.getBlue(), null);
        var bhsv = Color.RGBtoHSB(b.getRed(), b.getGreen(), b.getBlue(), null);
        // return Math.sqrt(hueDistance * hueDistance * 0.8f + saturationDistance *
        // saturationDistance * 0.1f + lightnessDistance * lightnessDistance * 0.1f);
        return 0;

    }

    public static double hue2rgb(double p, double q, double t) {
        if (t < 0) {
            t += 1;
        }
        if (t > 1) {
            t -= 1;
        }
        if (t < 1. / 6) {
            return p + (q - p) * 6 * t;
        }
        if (t < 1. / 2) {
            return q;
        }
        if (t < 2. / 3) {
            return p + (q - p) * (2. / 3 - t) * 6;
        }
        return p;
    }

    public static Color hue2color(double p, double q, double t) {
        return new Color((int) hue2rgb(p, q, t));
    }

    HSL rgb2hsl(float r, float g, float b) {
        HSL result = new HSL();

        r /= 255;
        g /= 255;
        b /= 255;
        float max = Math.max(Math.max(r, g), b);
        float min = Math.min(Math.min(r, g), b);

        result.h = result.s = result.l = (max + min) / 2;
        if (max == min) {
            result.h = result.s = 0; // achromatic
        } else {
            float d = max - min;
            result.s = (result.l > 0.5) ? d / (2 - max - min) : d / (max + min);
            if (max == r) {
                result.h = (g - b) / d + (g < b ? 6 : 0);
            } else if (max == g) {
                result.h = (b - r) / d + 2;
            } else if (max == b) {
                result.h = (r - g) / d + 4;
            }
            result.h /= 6;
        }
        return result;
    }


}

class ThreadPOolTest {
    ExecutorService executor = Executors.newFixedThreadPool(2);


    void longRunTask(int id) {
        while (true) {
            System.out.println("hi from " + id);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }


    void test() {
        for (int i = 0; i < 10; i++) {
            final var x = i;
            executor.submit(() -> longRunTask(x));
        }
    }
}


public class Main {

    public static void main(String[] args) throws IOException {

        var executor = Executors.newFixedThreadPool(3);

        var segmeter = new PyramidSegmenter();

        var traverser = new Traverser(new File(args[0]));
        var list = new ArrayList<Future<?>>();

        traverser.traverse(path -> {
            if (path.toString().endsWith(".2.png")) {
                return;
            }

            var it = executor.submit(() -> {
                try {
                    System.out.println(path);
                    var handled = segmeter.handle(ImageIO.read(path));
                    ImageIO.write(handled, "png", new File(path.toString() + ".2.png"));
                } catch (Exception e) {
                    e.printStackTrace();

                    // throw new Error(e);
                    // e.printStackTrace();
                    // System.err.println(e.getMessage());

                    // throw new RuntimeException(e);
                }
            });
            list.add(it);

        });

        for (var x : list) {
            try {
                x.get();
            } catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }

        }
        executor.shutdown();
        // ImageHandler ih = null;
        // if (args.length > 0) {
        // ih = new BayerDitheringFacade();
        // } else {
        // ih = new ErrorDiffusionFacade();
        // }

        // var innfolder = new File("imgs");

        // innfolder.mkdir();
        // new AllImagesProcessor(innfolder, ih).run();
    }
}

interface RangeLimiter {
    // int getLimitBy
}
