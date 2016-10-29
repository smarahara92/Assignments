import java.util.List;
import java.util.LinkedList;

public class Tokenizer {

	private String expression;
	private Token [] tokens;

	public Tokenizer(String expression) {
		this.expression = expression;
	}

	public Token getGroupTypeToken(String expr, int pos) throws Exception {
		if(expr.charAt(pos) == '(') {
			int bracecount = 1;
			int x = pos + 1;
			for(; x < expr.length() && bracecount > 0; x++) {
				char currentchar = expr.charAt(x);
				if(currentchar == '(') {
					bracecount += 1;
				} else if(currentchar == ')') {
					bracecount -= 1;
				}
			}
			if(bracecount == 0) {
				return new Token(expr.substring(pos, x), Token.GROUP);
			} else {
				throw new Exception("Matching braces mismatch!!!!!!!!!");
			}
		} else {
			throw new Exception("Expecting '(' character at the begining of the expression!!!!!!!!!");
		}
	}

	public Token [] getTokens() throws Exception {
		if(this.tokens == null) {
			List<Token> tokenlist = new LinkedList<>();
			for(int x = 0; x < this.expression.length(); x++) {
				char currentchar = this.expression.charAt(x);
				if(currentchar == ' ') {
					continue;
				}else if(currentchar == '(') {
					Token tk = getGroupTypeToken(this.expression, x);
					tokenlist.add(tk);
					x += tk.toString().length();
				} else if(currentchar == ')') {
					throw new Exception("Extra ')' character in the middle of the expression!!!!!!!!!");
				} else if(currentchar >= '0' && currentchar <= '9') {
					tokenlist.add(new Token(currentchar + "", Token.INTEGER));
				} else if(currentchar == '+' || currentchar == '-' || currentchar == '*' || currentchar == '/') {
					tokenlist.add(new Token(currentchar + "", Token.OPERATOR));
				} else {
					throw new Exception("Unexpected character in the middle of the expression!!!!!!!!");
				}
			}
			this.tokens = tokenlist.toArray(new Token[tokenlist.size()]);
		}
		return this.tokens;
	}

	public static void main(String [] args) {
		try {
			String expr1 = "(1 + 2) * 5 / 2 + 5 - 2 * 12 / 3";
			String expr2 = "1 + 2 + (3 + 4) + (5 * 6) + 7";
			Tokenizer t = new Tokenizer(expr1);
			Token [] tokens = t.getTokens();
			for(int x = 0; x < tokens.length; x++) {
				System.out.print(tokens[x]);
			}
			System.out.println("");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
