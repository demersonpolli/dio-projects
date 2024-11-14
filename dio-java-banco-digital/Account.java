package bancodigital;

/**
 * Defines a bank account.
 * 
 * @author Demerson Andre Polli
 * @since 1.0 (November 1st, 2024)
 */
public abstract class Account {
	private int number;
	private int ownerID;
	protected double balance;
	
	/**
	 * Constructs a class with number, owner ID and initial balance.
	 * @param number The number of the account.
	 * @param ownerID The account owner's ID.
	 * @param balance The initial account balance.
	 */
	public Account(int number, int ownerID, double balance) {
		this.number = number;
		this.ownerID = ownerID;
		this.balance = balance;
	}

	/**
	 * Gets the account number.
	 * 
	 * @return The number of the account.
	 */
	public int accountNumber() {
		return this.number;
	}
	
	/**
	 * Get the account's owner ID.
	 * @return The ID of the account's owner.
	 */
	public int accountOwnerID() {
		return this.ownerID;
	}
	
	/**
	 * Get the account balance.
	 * 
	 * @return The account balance.
	 */
	public double accountBalance() {
		return this.balance;
	}
	
	/**
	 * Put a given amount of money in the account.
	 * 
	 * @param value The amount of money to put in the account.
	 */
	public abstract void deposit(double value);

	/**
	 * Transfer a given amount of money to another account. Throws an 
	 * Exception if the balance is less than the value.
	 * 
	 * @param destiny The {@link Account} that will receive the money.
	 * @param value The money amount to be transfered.
	 * @throws Exception A message error if the account doesn't have 
	 * sufficient balance.
	 */
	public abstract void transfer(Account destiny, double value) throws Exception;
	
	/**
	 * Withdraw a given amount of money from the account. Throws an
	 * Exception if the balance is less than the value.
	 * 
	 * @param value The money amount to withdraw from the account.
	 * @throws Exception A message error if the account doesn't have
	 * sufficient balance.
	 */
	public abstract void withdraw(double value) throws Exception;
}
