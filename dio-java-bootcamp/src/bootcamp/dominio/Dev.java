package bootcamp.dominio;

import java.util.*;

/**
 * Class that defines a developer.
 */
public class Dev {
    private String name;
    private Set<Contents> subscriptedContents = new LinkedHashSet<>();
    private Set<Contents> finishedContents = new LinkedHashSet<>();
    
    public Dev(String name, Bootcamp bootcamp) {
    	this.name = name;
    	this.subscribeBootcamp(bootcamp);
    }

    /**
     * Calculate the eXperience Points.
     * 
     * @return The eXperience Points.
     */
    public double calculateTotalXP() {
        Iterator<Contents> iterator = this.finishedContents.iterator();
        double sum = 0;
        while(iterator.hasNext()){
            double next = iterator.next().calculateXP();
            sum += next;
        }
        
        return sum;
    }
    
    /**
     * Get the set of finished contents.
     * 
     * @return The set of finished contents.
     */
    public Set<Contents> getFinishedContents() {
        return this.finishedContents;
    }

    /**
     * Get the developer name.
     * 
     * @return The developer name.
     */
    public String getName() {
        return this.name;
    }
    
    /**
     * Get the subscripted contents.
     * 
     * @return The subscripted contents.
     */
    public Set<Contents> getSubscriptedContents() {
        return this.subscriptedContents;
    }

    /**
     * Progress n-steps in the boot-camp.
     * 
     * @param n Number of steps to progress.
     */
    public void progress(int n) {
    	int amount = (n <= 0)? 1: n;
    	
    	for (int i = 0; i < amount; i++)
    		progress();
    }
    
    /**
     * Progress one step in the boot-camp.
     */
    public void progress() {
        Optional<Contents> contents = this.subscriptedContents.stream().findFirst();
        if(contents.isPresent()) {
            this.finishedContents.add(contents.get());
            this.subscriptedContents.remove(contents.get());
        }
        else {
            System.err.println("Você não está matriculado em nenhum conteúdo!");
        }
    }

    /**
     * Set the developer name.
     * @param name The developer name.
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * Set the subscripted contents.
     * 
     * @param subscriptedContents A set of subscripted contents.
     */
    public void setSubscriptedContents(Set<Contents> subscriptedContents) {
        this.subscriptedContents = subscriptedContents;
    }
    
    /**
     * Subscribe the developer in a boot-camp.
     * 
     * @param bootcamp The boot-camp to subscribe.
     */
    public void subscribeBootcamp(Bootcamp bootcamp) {
        this.subscriptedContents.addAll(bootcamp.getContents());
        bootcamp.getSubscriptedDevs().add(this);
    }

    /**
     * Sets the finished contents.
     * 
     * @param finishedContents The set of finished contents.
     */
    public void setFinishedContents(Set<Contents> finishedContents) {
        this.finishedContents = finishedContents;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Dev dev = (Dev) o;
        return Objects.equals(name, dev.name) && Objects.equals(subscriptedContents, dev.subscriptedContents) && Objects.equals(finishedContents, dev.finishedContents);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name, subscriptedContents, finishedContents);
    }
}