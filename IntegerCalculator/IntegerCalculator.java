import java.util.Map;
import java.util.HashMap;
import java.util.Scanner;

public class IntegerCalculator {

	private static final Map<String, Integer> operatorPrecedence = new HashMap<>();
	static {
		operatorPrecedence.put("*", new Integer(1));
		operatorPrecedence.put("/", new Integer(1));
		operatorPrecedence.put("+", new Integer(2));
		operatorPrecedence.put("-", new Integer(2));
	}

	private String expression;
	private int result;

	public IntegerCalculator(String expression) throws Exception {
		setExpression(expression);
	}

	public void setExpression(String expression) throws Exception {
		this.expression = expression;
		result = Integer.parseInt(evaluate(expression).token);
	}

	public String getExpression() {
		return this.expression;
	}

	public int getResult() {
		return this.result;
	}

	private int evaluate(Token [] tokenlist, int startpos, int endpos) throws Exception {
		if((endpos - startpos) == 1) {
			if(tokenlist[startpos].type == Token.INTEGER) {
				return Integer.parseInt(tokenlist[startpos].token);
			}
			throw new Exception("Malformed expression!!!!!!!!!");
		}
		String operator = "";
		int pos = startpos + 1;
		int priority = 0;
		for(int x = pos; x < endpos; x += 2) {
			if(tokenlist[x].type == Token.OPERATOR) {
				int priory = IntegerCalculator.operatorPrecedence.get(tokenlist[x].token);
				if(priory > priority) {
					operator = tokenlist[x].token;
					pos = x;
					priority = priory;
				}
			} else {
				throw new Exception("Malformed expression!!!!!!!!!");
			}
		}
		if((pos > 0) && (pos % 2 == 1)) {
			if(operator.equals("*")) return evaluate(tokenlist, startpos, pos) * evaluate(tokenlist, pos + 1, endpos);
			else if(operator.equals("/")) return evaluate(tokenlist, startpos, pos) / evaluate(tokenlist, pos + 1, endpos);
			else if(operator.equals("+")) return evaluate(tokenlist, startpos, pos) + evaluate(tokenlist, pos + 1, endpos);
			else if(operator.equals("-")) return evaluate(tokenlist, startpos, pos) - evaluate(tokenlist, pos + 1, endpos);
			else throw new Exception("Malformed operator!!!!!!!!!");
		} else {
			throw new Exception("Malformed expression!!!!!!!!!");
		}
	}

	private Token evaluate(String expr) throws Exception {
		Tokenizer tk = new Tokenizer(expr);
		Token [] tokenlist = tk.getTokens();
		for(int x = 0; x < tokenlist.length; x++) {
			if(tokenlist[x].type == Token.GROUP) {
				tokenlist[x] = evaluate(tokenlist[x].token);
			}
		}
		return new Token(evaluate(tokenlist, 0, tokenlist.length) + "", Token.INTEGER);
	}

	public static void main(String [] args) {
		Scanner sc = new Scanner(System.in);
		System.out.print("Enter the expression :: ");
		String expr = sc.nextLine();
		try {
			IntegerCalculator ic = new IntegerCalculator(expr);
			System.out.println("Result :: " + ic.getResult());
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
