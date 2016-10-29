public class Token {

	public static final int INTEGER = 1;
	public static final int OPERATOR = 2;
	public static final int GROUP = 3;

	public final String token;
	public final int type;

	public Token(String token, int type) {
		if(type == Token.GROUP) {
			if(token.charAt(0) == '(' && token.charAt(token.length() - 1) == ')') {
				token = token.substring(1, token.length() - 1);
			}
		}
		this.token = token;
		this.type = type;
	}

	public String toString() {
		if(this.type == Token.GROUP) {
			return '(' + token + ')';
		} else {
			return token;
		}
	}

}
