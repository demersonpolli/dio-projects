package bootcamp;

import bootcamp.dominio.Bootcamp;
import bootcamp.dominio.Course;
import bootcamp.dominio.Dev;
import bootcamp.dominio.Mentoring;

import java.time.LocalDate;

public class Main {
	public static void main(String[] args) {
		Course course1 = new Course();
        course1.setTitle("Curso de Java");
        course1.setDescription("Curso introdutório de Java.");
        course1.setWorkLoad(8);

        Course course2 = new Course();
        course2.setTitle("Curso de JavaScript");
        course2.setDescription("Introdução ao JavaScript.");
        course2.setWorkLoad(4);

        Mentoring mentoring = new Mentoring();
        mentoring.setTitle("Mentoria de Java");
        mentoring.setDescription("Mentoria de Java em nível introdutório.");
        mentoring.setDate(LocalDate.now());

        Bootcamp bootcamp = new Bootcamp();
        bootcamp.setName("Bootcamp Java Developer");
        bootcamp.setDescription("Descrição Bootcamp Java Developer");
        bootcamp.getContents().add(course1);
        bootcamp.getContents().add(course2);
        bootcamp.getContents().add(mentoring);

        Dev dev1 = new Dev("Camila", bootcamp);
        System.out.println(String.format("Conteúdos Inscritos %s: %s", dev1.getName(), dev1.getSubscriptedContents()));

        dev1.progress(2);
        
        System.out.println("-");
        System.out.println(String.format("Conteúdos Inscritos %s: %s", dev1.getName(), dev1.getSubscriptedContents()));
        System.out.println(String.format("Conteúdos Concluídos %s: %s", dev1.getName(), dev1.getFinishedContents()));
        System.out.println("XP:" + dev1.calculateTotalXP());

        System.out.println("-------");

        Dev dev2 = new Dev("Joao", bootcamp);
        System.out.println(String.format("Conteúdos Inscritos %s: %s", dev2.getName(), dev2.getSubscriptedContents()));

        dev2.progress(3);
        
        System.out.println("-");
        System.out.println(String.format("Conteúdos Inscritos %s: %s", dev2.getName(), dev2.getSubscriptedContents()));
        System.out.println(String.format("Conteúdos Concluidos %s: %s", dev2.getName(), dev2.getFinishedContents()));
        System.out.println("XP:" + dev2.calculateTotalXP());
	}
}