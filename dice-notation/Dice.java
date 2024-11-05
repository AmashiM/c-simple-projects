public class Dice {

    // using a class here since I think enums are bs since you can't use math operators on them.
    public class TokenType {
        public final int TYPE_SPACE = 0;
        public final int TYPE_DICE = 1;
        public final int TYPE_NUM = 2;
        public final int TYPE_KEEP = 3;
        public final int TYPE_KEEP_HIGH = 4;
        public final int TYPE_KEEP_LOW = 5;
        public final int TYPE_GROUP_START = 6;
        public final int TYPE_GROUP_END = 7;
        public final int TYPE_ADD = 8;
        public final int TYPE_SUB = 9;
        public final int TYPE_MULT = 10;
        public final int TYPE_DIV = 11;
        public final int TYPE_EMPTY = 12;
    };

    class PairToken {
        public int type;
        public int value;

        public PairToken(int type, int value){
            this.type = type;
            this.value = value;
        }
    }

    public class DiceNotation {
        private String text;
        private int length;
        private PairToken tokens[];

        private static boolean IsDigit(char c){
            return '0' <= c && c <= '9';
        }

        public DiceNotation(String text){
            this.text = text;
            length = text.length();
            tokens = new PairToken[length];
            Process();
        }

        private void CleanTokens(){
            for(int i = 0; i < length; i++){
                tokens[i].type = -1;
                tokens[i].value = 0;
            }
        }

        private void ProcessText(){
            char c;

            for(int i = 0; i < length; i++){
                c = text.charAt(i);

                switch(c){
                    case 'd':
                        break;
                    case '+':
                    case '-':
                    case '/':
                    case '*':
                        break;
                    case ' ':
                        break;
                    default: {
                        if(DiceNotation.IsDigit(c)){

                        }
                    }; break;
                }
            }
        }

        private void Process(){
            CleanTokens();
            ProcessText();
        }
    }


    


    public static long DiceNotation(String text){
        final int length = text.length();

        PairToken tokens[] = new PairToken[length];

        for(int i = 0; i < length; i++){
            char c = text.charAt(i);
            switch(c){
                case ' ':
                    break;
            }
        }

        return 0;
    }


    public static void Main(String[] args){


    }
}