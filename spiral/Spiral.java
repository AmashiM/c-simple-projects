
import java.util.function.Function;

import javax.security.auth.callback.Callback;

// for my curiosity I'm taking a java class rn so I was gonna write a java version.

public class Spiral {

    public static int SpiralCallback(int value){
        return value + 1;
    };

    public static String CellToString(int value) {
        return String.format("%d", value);
    };

    public static void PrintGrid(int[][] array) {
        for(int i = 0; i < array.length; i++){
            int[] row = array[i];
            final int row_length = row.length;
            System.out.print('[');
            for(int q = 0; q < row_length; q++){
                int cell = row[q];
                String text = Spiral.CellToString(cell);
                System.out.print(text);
                if(q + 1 < row_length){
                    System.out.print(',');
                }
            }
            System.out.println(']');
        }
    }

    public static void ForEach(int[][] array){
        int size = Math.max(array.length, array[0].length);
        /**md
         * `r/s(x|y)/` = min
         * `r/b(x|y)/` = max
         */
        int x, y, min_x, max_x, min_y, max_y;
        x = -1;

        max_x = max_y = size;
        y = min_x = min_y = 0;

        // moves in this direction: left, down, right, up
        // if you want to change directions, you need to change the code but main thing is that I do use unsigned numbers here so you'll need to adjust for signed values or use logic.
        // iterations should be half the size
        for(int iterations = 0; iterations < (size / 2); iterations++){
            int change;
            for(char direction = 0; direction < 4; direction++){
                int orientation; // 1 == horizontal, 0 == vertical
                int length; // length(l) can be either width or height

                if(direction < 2){
                    change = 1;
                } else {
                    change = -1;
                }

                orientation = direction & 0b000000001;

                if(orientation == 0){
                    length = max_x - min_x; // width
                } else {
                    length = max_y - min_y; // height
                }

                // process movement for length(l) where m is the steps taken
                for(int movement = 0; movement < length; movement++){
    
                    if(orientation == 0){
                        x = x + change;
                    } else {
                        y = y + change;
                    }
    
                    // get value and pass to callback
                    // java: can't use pointers so we're gonna return the new value of the int.
                    array[y][x] = Spiral.SpiralCallback(array[y][x]);
                }
    
                switch(direction){
                    default: System.out.println("something unexpected happened"); break;
                    case 0: min_y++; break;
                    case 1: max_x--; break;
                    case 2: max_y--; break;
                    case 3: min_x++; break;
                }
            }

            Spiral.PrintGrid(array);
            System.out.print('\n');
        }
    }

    public static void main(String[] args){

        int array[][] = new int[10][10];

        Spiral.ForEach(array);
    }
}