package bancodigital;

public class Banking extends Account {
	private double specialLimit;

	public Banking(int number, int ownerID, double balance) {
		super(number, ownerID, balance);
		
		this.specialLimit = 0.0;
	}
	
	public double accountAvailableMoney() {
		return this.balance + this.specialLimit;
	}
	
	public double getSpecialLimit() {
		return this.specialLimit;
	}
	
	public void setSpecialLimit(double limit) throws Exception {
		if (limit < 0)
			throw new Exception("Limite do cheque especial precisa ser maior ou igual a zero.");
		
		this.specialLimit = limit;
	}

	@Override
	public void deposit(double value) {
		this.balance += value;
	}

	@Override
	public void transfer(Account destiny, double value) throws Exception {
		if (value < this.balance + this.specialLimit)
			throw new Exception("Saldo insuficiente para transferência.");
		
		this.balance -= value;
		destiny.deposit(value);
	}

	@Override 
	public void withdraw(double value) throws Exception {
		if (value < this.balance + this.specialLimit)
			throw new Exception("Saldo insuficiente para saque.");
		
		this.balance -= value;
	}
}
