package bancodigital;

import java.util.HashMap;

/**
 * Defines the structure of a Bank.
 * 
 * @author Demerson Andre Polli
 * @since 1.0 (November 1st, 2024).
 */
public class Bank {
	private String name;
	private int id;
	private HashMap<Integer, Agency> agencies;
	
	/**
	 * Constructor that sets the bank name and bank ID.
	 * @param nome The bank name.
	 * @param codigo The bank ID.
	 */
	public Bank(String name, int id) {
		this.name = name;
		this.id = id;
		
		agencies = new HashMap<>();
	}
	
	/**
	 * Insert an agency in the list of agencies of the bank. Gets an object
	 * of the class {@link Agency} and an ID.
	 * 
	 * @param agency An {@link Agency} object.
	 */
	public void addAgency(Agency agency) {
		agencies.put(agency.agencyID(), agency);
	}

	/**
	 * Gets the ID of the bank.
	 * 
	 * @return The ID of the bank.
	 */
	public int bankID() {
		return this.id;
	}

	/**
	 * Gets the name of the bank.
	 * 
	 * @return The name of the bank.
	 */
	public String bankName() {
		return this.name;
	}
	
	/**
	 * Gets an {@link Agency} object if there is an agency with the given ID,
	 * otherwise throws an exception.
	 * 
	 * @param id ID of the agency.
	 * @return An {@link Agency} object if the agency exists.
	 * @throws Exception An error message if the agency does not exists.
	 */
	public Agency getAgency(int id) throws Exception {
		if (!agencies.containsKey(id)) {
			throw new Exception("Agência não existe.");
		}
		return agencies.get(id);
	}

	/**
	 * Remove the agency with the given code from the list of agencies.
	 * 
	 * @param id The ID of the agency to be removed.
	 */
	public void removeAgency(int id) {
		if (agencies.containsKey(id))
			agencies.remove(id);
	}
}
