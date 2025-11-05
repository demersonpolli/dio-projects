package bootcamp.dominio;

import java.time.LocalDate;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Objects;
import java.util.Set;

/**
 * Class that defines the boot-camp.
 */
public class Bootcamp {
    private String name;
    private String description;
    private final LocalDate initialDate = LocalDate.now();
    private final LocalDate finalDate = initialDate.plusDays(45);
    private Set<Dev> subscriptedDevs = new HashSet<>();
    private Set<Contents> contents = new LinkedHashSet<>();

    /**
     * Get the boot-camp contents.
     * 
     * @return The boot-camp contents.
     */
    public Set<Contents> getContents() {
        return this.contents;
    }
    
    /**
     * Get the boot-camp description.
     * 
     * @return The boot-camp description.
     */
    public String getDescription() {
        return this.description;
    }
    
    /**
     * Get the final boot-camp date.
     * 
     * @return The final boot-camp date.
     */
    public LocalDate getFinalDate() {
    	return this.finalDate;
    }
    
    /**
     * Get the initial boot-camp date.
     * 
     * @return The initial boot-camp date.
     */
    public LocalDate getInitialDate() {
    	return this.initialDate;
    }
    
    /**
     * Get the boot-camp name.
     * 
     * @return The boot-camp name.
     */
    public String getName() {
        return this.name;
    }
    
    /**
     * Get subscripted developers.
     * 
     * @return A set of subscripted developers.
     */
    public Set<Dev> getSubscriptedDevs() {
    	return this.subscriptedDevs;
    }
    
    /**
     * Get the boot-camp description.
     * 
     * @param description The boot-camp description.
     */
    public void setDescription(String description) {
        this.description = description;
    }    

    /**
     * Set the boot-camp name.
     * 
     * @param nome The boot-camp name.
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * Set the subscripted developers.
     * 
     * @param subscriptedDevs A set of subscripted developers.
     */
    public void setSubscriptedDevs(Set<Dev> subscriptedDevs) {
        this.subscriptedDevs = subscriptedDevs;
    }

    /**
     * Set the boot-camp contents.
     * 
     * @param contents The boot-camp contents.
     */
    public void setContents(Set<Contents> contents) {
        this.contents = contents;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Bootcamp bootcamp = (Bootcamp) o;
        return Objects.equals(name, bootcamp.name) && Objects.equals(description, bootcamp.description) && Objects.equals(initialDate, bootcamp.initialDate) && Objects.equals(finalDate, bootcamp.finalDate) && Objects.equals(subscriptedDevs, bootcamp.subscriptedDevs) && Objects.equals(contents, bootcamp.contents);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name, description, initialDate, finalDate, subscriptedDevs, contents);
    }
}