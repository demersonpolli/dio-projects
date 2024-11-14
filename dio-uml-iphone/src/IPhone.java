public abstract class IPhone implements AparelhoTelefonico, NavegadorInternet, ReprodutorMusical {
    private boolean telefoneTocando;
    private boolean musicaTocando;
    private String numeroTelefone;
    private String musicaAtual;

    public boolean telefoneTocando() {
        return telefoneTocando;
    }

    public boolean musicaTocando() {
        return musicaTocando;
    }

    public String numeroTelefone() {
        return numeroTelefone;
    }

    public String musicaAtual() {
        return musicaAtual;
    }

    // Métodos da interface AparelhoTelefonico
    public abstract void atender();
    public abstract void iniciarCorreioVoz();
    public abstract boolean ligar(String numero);

    // Métodos da interface NavegadorInternet
    public abstract void adicionarNovaAba();
    public abstract void atualizarPagina();
    public abstract void exibirPagina(String url);

    // Métodos da interface ReprodutorMusical
    public abstract void pausar();
    public abstract void selecionarMusica(String musica);
    public abstract void tocar();

    // Construtor da classe abstrata
    public IPhone() {
        telefoneTocando = false;
        musicaTocando = false;
        numeroTelefone = "";
        musicaAtual = "";
    }
}
