import java.util.Locale;
import java.util.Scanner;

public class ContaTerminal {
    private Scanner scanner = new Scanner(System.in).useLocale(Locale.US);

    public static void main(String[] args) throws Exception {
        ContaTerminal object = new ContaTerminal();


        System.out.println("*** DIO Bank ***");
        System.out.println();

        System.out.println(object.readData());
    }

    private String readData() {
        System.out.println("Digite sua agência: ");
        String agencia = scanner.nextLine();

        System.out.println("Digite o número da conta: ");
        int conta = scanner.nextInt();

        System.out.println("Digite o nome do cliente: ");
        String nome = scanner.nextLine();

        System.out.println("Digite o saldo inicial: ");
        double saldo = scanner.nextDouble();

        return String.format("Olá %s, obrigado por criar uma conta em nosso banco, " +
                             "sua agência é %s, conta %d e seu saldo %.2f já " +
                             "está disponível para saque",
            nome, agencia, conta, saldo);
    }
}
