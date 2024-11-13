package bootcamp.dominio;

public class Course extends Contents {

    private int workLoad;

    @Override
    public double calculateXP() {
        return STANDARD_XP * workLoad;
    }
    
    /**
     * Get the work load of the course.
     * @return The work load (in hours) of the course.
     */
    public int getWorkLoad() {
        return this.workLoad;
    }

    /**
     * Set the work load of the course.
     * 
     * @param workLoad The work load of the course.
     */
    public void setWorkLoad(int workLoad) {
        this.workLoad = workLoad;
    }

    @Override
    public String toString() {
        return "Curso{" +
                "titulo='" + getTitle() + '\'' +
                ", descricao='" + getDescription() + '\'' +
                ", cargaHoraria=" + workLoad +
                '}';
    }
}