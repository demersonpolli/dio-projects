package bancodigital;

import java.util.HashMap;

/**
 * Defines the structure of an agency of a bank.
 * 
 * @author Demerson Andre Polli
 * @since 1.0 (November 1st, 2024)
 */
public class Agency {
	private int id;
	private String name;
	private String address;
	private String phone;
	
	private HashMap<Integer, Account> accounts;
	
	/**
	 * Constructor that sets the name, ID, address and phone of an agency.
	 * 
	 * @param name The name of the agency.
	 * @param id   The ID of the agency.
	 * @param address The address of the agency.
	 * @param phone The phone number of the agency.
	 */
	public Agency(String name, int id, String address, String phone) {
		this.name = name;
		this.id = id;
		this.address = address;
		this.phone = phone;
		
		accounts = new HashMap<>();
	}
	
	/**
	 * Adds an account in the agency.
	 * 
	 * @param account An object of the class {@link Account}.
	 */
	public void addAccount(Account account) {
		accounts.put(account.accountNumber(), account);
	}
	
	/**
	 * Gets the address of the agency.
	 * 
	 * @return The address of the agency.
	 */
	public String agencyAddress() { 
		return this.address;
	}
	
	/**
	 * Get the ID of the agency.
	 * 
	 * @return The ID of the agency.
	 */
	
	public int agencyID() {
		return this.id;
	}
	
	/**
	 * Get the name of the agency.
	 * 
	 * @return The name of the agency.
	 */
	public String agencyName() {
		return this.name;
	}
	
	/**
	 * Gets the agency phone number.
	 * 
	 * @return The phone number of the agency.
	 */
	public String agencyPhone() {
		return this.phone;
	}

	/**
	 * Get the account of the agency with a given number. 
	 * If the account does not exist it throws an exception.
	 * 
	 * @param number The number of the account to search.
	 * 
	 * @return An {@link Account} object if the account exists.
	 * @throws Exception An error message if the account does not exist.
	 */
	public Account getAccount(int number) throws Exception {
		if (!accounts.containsKey(number))
			throw new Exception("Conta inexistente.");
		
		return accounts.get(number);
	}
	
	/**
	 * Removes the account with the given number from the list of accounts.
	 * 
	 * @param number The number of the account to search.
	 */
	public void removeAccount(int number) {
		if (accounts.containsKey(number))
			accounts.remove(number);
	}
}
