import org.w3c.dom.Notation;

public class Dice {

    public static long Roll(int amount, int sides, boolean keep_highest, boolean keep_lowest){
        long total = 0;

        int mode = 0;

        if(keep_highest){
            mode = 1;
        } else if(keep_lowest){
            mode = 2;
        }

        for(int i = 0; i < amount; i++){
            int result = (int)(Math.floor(Math.random() * sides));

            switch (mode) {
                default:
                case 0:
                    total = total + result;
                    break;
                case 1:
                    if(total < result){
                        total = result;
                    }
                    break;
                case 2:
                    if(total > result){
                        total = result;
                    }
                    break;
            }
        }

        return total;
    }

    public class DiceToken {
        public int data[5];

        public static int TYPE = 0;
        public static int DICE_SIDES = 1;
        public static int DICE_AMOUNT = 2;
        public static int DICE_KEEP_HIGHEST = 3;
        public static int DICE_KEEP_LOWEST = 4;

        public static int GROUP_MODE = 1; // 1 = start, 2 = end
        public static int GROUP_START_INDEX = 2;
        public static int GROUP_END_INDEX = 3;
        
        public static final int TYPE_SPACE = 0; // space
        public static final int TYPE_DICE = 1; // dice
        public static final int TYPE_KEEP = 2; // keep
        public static final int TYPE_KEEP_LOWEST = 3; // keep lowest
        public static final int TYPE_KEEP_HIGHEST = 4; // keep highest
        public static final int TYPE_ADD = 5; // add
        public static final int TYPE_SUB = 6; // subtract
        public static final int TYPE_DIV = 7; // divide
        public static final int TYPE_MULT = 8; // multiply
        public static final int TYPE_GROUP = 9; // group - parenthesis
        public static final int TYPE_NUM = 10; // number
    }

    public static void Notation(String text){
        final int length = text.length();

        int token_types[] = new int[length];

        for(int i = 0; i < length; i++){
            char c = text.charAt(i);
            c.

        }

        int tokens[][] = new int[length][5];

        boolean processing_number = false;
        boolean processing_text = false;
        int current_number = 0;
        char current_text[] = { 0, 0 };

        for(int i = 0; i < length; i++){
            char c = text.charAt(i);

            if('0' <= c && c <= '9'){
                if(processing_number){
                    current_number = (current_number * 10) + ((int)c - (int)'0');
                } else {
                    processing_number = true;
                    current_number = (c - '0');
                }
            } else {
                if(processing_number){
                    processing_number = false;
                }
            }
        }
    }

    public static void PrintGrid(int[][] array) {
        for(int i = 0; i < array.length; i++){
            int[] row = array[i];
            final int row_length = row.length;
            System.out.print('[');
            for(int q = 0; q < row_length; q++){
                int cell = row[q];
                System.out.print(String.format("%d", cell));
                if(q + 1 < row_length){
                    System.out.print(',');
                }
            }
            System.out.println(']');
        }
    }

    public static void main(String[] args){
        int out = (int)Roll(20, 20, false, false);

        System.out.println(String.format("Got: %d", out));

        Notation("1d20");
    }
}