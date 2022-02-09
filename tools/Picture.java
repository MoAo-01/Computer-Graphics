package tools;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;

/**
  * The Picture class provides methods to manipulate digital images. 
  * The set of values is a two-dimensional matrix of Color values. 
  * It allows to create an image either blank or from a file,
  * set the value of a pixel to a given color, and get the color of 
  * a given pixel using the RGB color format. 
  */
public final class Picture { 
    private BufferedImage image;
    private JFrame frame;
    private String filename;
    private final int width, height;

   /**
     * Creates a width-by-height picture, with width columns
     * and height rows, where each pixel is black.
     *
     * @param width the width of the picture
     * @param height the height of the picture
     * @throws IllegalArgumentException if width is negative or zero
     * @throws IllegalArgumentException if height is negative or zero
     */
    public Picture(int width, int height) {
        if (width  <= 0)
            throw new IllegalArgumentException("width must be positive");
        if (height <= 0)
            throw new IllegalArgumentException("height must be positive");
        this.width  = width;
        this.height = height;
        image = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
    }

   /**
     * Creates a picture by reading an image from a file name.
     *
     * @param name the name of the .png or .jpg file
     * @throws IllegalArgumentException if cannot read image
     * @throws IllegalArgumentException if name is null
     */
    public Picture(String name) {
        if (name == null)
            throw new IllegalArgumentException("constructor arg is null");
        this.filename = name;
        try {
            File file = new File(name);
            if (file.isFile())
                image = ImageIO.read(file);
            if (image == null)
                throw new IllegalArgumentException("could not read image: " + name);
            width  = image.getWidth(null);
            height = image.getHeight(null);
        }
        catch (IOException ioe) {
            throw new IllegalArgumentException("could not open image: " + name, ioe);
        }
    }

   /**
     * Displays the picture in a window on the screen.
     */
    public void show() {
        if (frame == null) {
            frame = new JFrame();
            frame.setContentPane(getJLabel());            
            frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
            if (filename == null)
                frame.setTitle(width + "x" + height);
            else
                frame.setTitle(filename);
            frame.setResizable(false);
            frame.pack();
            frame.setVisible(true);
        }
        frame.repaint();
    }
    
    private JLabel getJLabel() {
        if (image == null) return null;
        ImageIcon pic = new ImageIcon(image);
        return new JLabel(pic);
    }

   /**
     * Returns the height of the picture.
     *
     * @return the height of the picture in pixels
     */
    public int height() {
        return height;
    }

   /**
     * Returns the width of the picture.
     *
     * @return the width of the picture in pixels
     */
    public int width() {
        return width;
    }
    
   /*
    *
    * CW3 Part C
	*
	*
    */
    public Color getColor(int col, int row) {
        validateColumnIndex(col);
        validateRowIndex(row);
        int rgb = getRGB(col, row);
        return new Color(rgb);
    }
    
    /*
    *
    * CW3 Part C
	*
	*
    */
    public void setColor(int col, int row, Color color) {
        validateColumnIndex(col);
        validateRowIndex(row);
        if (color == null) throw new IllegalArgumentException("color argument is null");
        int rgb = color.getRGB();
        setRGB(col, row, rgb);
    }
    
    private void validateRowIndex(int row) {
        if (row < 0 || row >= height())
            throw new IllegalArgumentException("row not between 0 and " + (height()- 1));
    }
    
    private void validateColumnIndex(int col) {
        if (col < 0 || col >= width())
            throw new IllegalArgumentException("col not be between 0 and " + (width()-1));
    }

    private int getRGB(int col, int row) {
        validateColumnIndex(col);
        validateRowIndex(row);
        return image.getRGB(col, row);
    }
    
    private void setRGB(int col, int row, int rgb) {
        validateColumnIndex(col);
        validateRowIndex(row);
        image.setRGB(col, row, rgb);
    }

   /**
     * Returns a string representation of this picture.
     *
     * @return a string representation of this picture
     */
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(width);
        sb.append("x");
        sb.append(height);
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                int rgb = image.getRGB(col, row);
                sb.append(rgb);
            }
            sb.append("\n");
        }
        return sb.toString().trim();
    }
    
    /**
     * Returns true if this picture is equal to the other picture.
     *
     * @param other the other picture
     * @return true if this picture is the same dimension as other
     *         and if all pixels have the same color; false otherwise
     */
    @Override
    public boolean equals(Object other) {
        if (other == this) return true;
        if (other == null) return false;
        if (other.getClass() != this.getClass()) return false;
        Picture that = (Picture) other;
        if (this.width()  != that.width())  return false;
        if (this.height() != that.height()) return false;
        for (int col = 0; col < width(); col++)
            for (int row = 0; row < height(); row++)
                if (this.getRGB(col, row) != that.getRGB(col, row))
                    return false;
        return true;
    }
    

}
