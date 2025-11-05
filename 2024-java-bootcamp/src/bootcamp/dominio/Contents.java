package bootcamp.dominio;

/**
 * Class that defines the contents of a boot-camp.
 */
public abstract class Contents {

    protected static final double STANDARD_XP = 10d;

    private String description;
    private String title;

    /**
     * Calculates the eXperience Points of the boot-camp.
     * @return The eXperience Points of the boot-camp.
     */
    public abstract double calculateXP();

    /**
     * Get the boot-camp description.
     * 
     * @return The boot-camp description.
     */
    public String getDescription() {
    	return this.description;
    }
    
    /**
     * Get the boot-camp title.
     * @return The boot-camp title.
     */
    public String getTitle() {
        return this.title;
    }

    /**
     * Set the boot-camp description.
     * 
     * @param description The boot-camp description.
     */
    public void setDescription(String description) {
    	this.description = description;
    }
    
    /**
     * Set the boot-camp title.
     * 
     * @param title The boot-camp title.
     */
    public void setTitle(String title) {
        this.title = title;
    }
}