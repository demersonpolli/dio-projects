package bootcamp.dominio;

import java.time.LocalDate;

/**
 * The class that defines a mentoring.
 */
public class Mentoring extends Contents {

    private LocalDate date;

    @Override
    public double calculateXP() {
        return STANDARD_XP + 20d;
    }

    /**
     * Get mentoring date.
     * @return The mentoring date.
     */
    public LocalDate getDate() {
        return this.date;
    }

    /**
     * Set the mentoring date.
     * 
     * @param date The mentoring date.
     */
    public void setDate(LocalDate date) {
        this.date = date;
    }

    @Override
    public String toString() {
        return "Mentoria{" +
                "titulo='" + getTitle() + '\'' +
                ", descricao='" + getDescription() + '\'' +
                ", data=" + date +
                '}';
    }
}