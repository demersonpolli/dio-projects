package bancodigital;

/**
 * This class implement an investing account.
 * 
 * @author Demerson Andre Polli
 * @since 1.0 (November 1st, 2024)
 */
public class Investing extends Account {

	/**
	 * Constructs an investing account with number, owner id, and balance.
	 * 
	 * @param number Account number.
	 * @param ownerID Account owner's ID.
	 * @param balance Account initial balance.
	 */
	public Investing(int number, int ownerID, double balance) {
		super(number, ownerID, balance);
	}

	@Override
	public void deposit(double value) {
		this.balance += value;
	}

	@Override
	public void transfer(Account destiny, double value) throws Exception {
		if (this.balance < value)
			throw new Exception("Saldo insuficiente para transferência.");
		
		this.balance -= value;
		destiny.deposit(value);
	}

	@Override
	public void withdraw(double value) throws Exception {
		if (this.balance < value)
			throw new Exception("Saldo insuficiente para saque.");
		
		this.balance -= value;
	}
}
