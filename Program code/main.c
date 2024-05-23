#include <stdio.h> 
#include <stdlib.h> // бібліотека для роботи з динамічною пам'яттю (і не тільки)
#include <Windows.h> // бібліотека, котра містить 2 функції які змінюють кодування консолі (потрібно на кирилицю)
#include <ctype.h> // підключив для використання функції tolower()

#define file_name "file.txt" // макрос, який зберігатиме назву файла, з яким працюватиме програма

// блок структур та enum, які використовуються в програмі
typedef struct Date { // структура, яка зберігатиме дату народження студента (рік, місяць, день)
	short int year, month, day;
} Date;

typedef enum {
	MALE, FEMALE // чоловік(0) чи жінка(1)
} Gender; 

typedef struct Student {
	char name[100], surname[100], middle_name[100], group[50]; // ім'я, прізвище, по-батькові, назва групи студента
	Date birthday; // народження (рік, місяць, день)
	Gender gender; // щоб знати стать людини
} Student;

typedef struct List { // основа програми буде однозв'язний список, куди будуть записуватись студенти
	Student student;
	struct List* next;
} List;
// кінець блока структур та enum, які використовуються в програмі


// прототипи усіх функцій, котрі використовуються в програмі
void cleanInputBuffer(); // функція для очищення буфера вводу
List* loadDB(const char*); // функція для читання файла з БД і записування результату в список
void saveDB(const char*, List*); // функція для запису даних (списка) у файл
void print_list(List*); // функція для виводу вузлів списка (вивід БД)
void add_student(List**); // функція для додавання нового студента у список
void delete_student(List**); // функція для видалення студента із списку за введеним ім'я та прізвищем
void lowercase(char[]); // функція, яка змінює рядок шляхом перенесення всіх символів рядка у нижній регістр
// кінець прототипів функцій


int main(void) {
	printf("Hi! Select your language for further data entry "
		"(press 1 - \033[1;34mU\033[0;33mA\033[0m, press 2 - \033[0;31mU\033[0;34mK\033[0m): ");
	short int choice = 0;
	do{
		if (scanf_s("%hd", &choice) != 1) printf("Entered incorrect value!\nTry again: ");
		else if (choice != 1 && choice != 2) printf("Entered incorrect number!\nTry again: ");
		cleanInputBuffer(); // очищення буфера вводу
	} while (choice != 1 && choice != 2);
	if (choice == 1) { SetConsoleCP(1251); SetConsoleOutputCP(1251); }// якщо користувач обрав UA
	
	// основний блок коду програми 
	List* student_list = loadDB(file_name);
	choice = 0;
	do{
		printf("\033[0;36mMain menu:\033[0m\n1. Display the list of students\n2. Add a new student\n"
			"3. Delete an existing student (by surname and name)\n4. Exit the program\n"
			"Your choice (press 1, 2, 3, 4): ");
		do {
			if (scanf_s("%hd", &choice) != 1) printf("Entered incorrect value!\nTry again: ");
			else if (choice != 1 && choice != 2 && choice != 3 && choice != 4) 
				printf("Entered incorrect number!\nTry again: ");
			cleanInputBuffer(); // очищення буфера вводу
		} while (choice != 1 && choice != 2 && choice != 3 && choice != 4); // обробка виключених ситуацій

		switch (choice){
		case 1: print_list(student_list); break;
		case 2: add_student(&student_list); break;
		case 3: delete_student(&student_list); break;
		case 4: saveDB(file_name, student_list); break;
		default:
			break;
		}

	} while (choice != 4);
	//кінець основного блоку програми

	List* temp = 0;
	while ((temp = student_list) != NULL) {
		student_list = student_list->next;
		free(temp);
	}
    // очищую пам'ять, виділену для списка в процесі роботи програми
	return 0;
}

void cleanInputBuffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

List* loadDB(const char* file) {
	FILE* fp;
	if (fopen_s(&fp, file, "r") != 0)
		return NULL; 	// якщо файл не було прочитано (швидше за все, через його відсутність)
	// якщо файл успішно відкрився, тоді створюємо список і записуємо туди елементи
	List* head = 0, *curr = 0; // кореневий вузол списка і curr для здійснення ітерації по списку
	Student st; // сюди будуть записуватись зчитані елементи з файла
	// пропускаємо заголовок текстового файлу (1 рядок файла), решту зчитуємо
	char buffer[65]; // для того, щоб пропустити 65 символів першого рядка файла і читати з наступного
	fgets(buffer, sizeof(buffer), fp);
	while (fscanf_s(fp, "%99s %99s %99s %49s %hd.%hd.%hd %d",
		st.surname, (unsigned)_countof(st.surname),
		st.name, (unsigned)_countof(st.name),
		st.middle_name, (unsigned)_countof(st.middle_name), // так спеціально потрібно прописати для MS Visual Studio 2022
		st.group, (unsigned)_countof(st.group),
		&st.birthday.year, &st.birthday.month, &st.birthday.day,
		(int*)&st.gender) == 8) {
		List* newnode = (List*)malloc(sizeof(List));
		if (NULL == newnode) { puts("Memory allocation error :("); break; }
		newnode->student = st; newnode->next = 0;
		if (NULL == head) curr = head = newnode; // якщо записуємо в порожній список
		else curr = curr->next = newnode;
	}
	fclose(fp);
	return head;
}

void print_list(List* head) {
	putchar('\n');
	List* temp = head;
	while (temp != NULL) {
		printf("Surname: %s, Name: %s, Group: %s, Gender (0 - man, 1 - girl): %d\n", temp->student.surname, 
			temp->student.name,temp->student.group, temp->student.gender);
		temp = temp->next;
	}
	putchar('\n');
}

void saveDB(const char* file, List* head) {
	FILE* fp;
	if (fopen_s(&fp, file, "w") != 0) { puts("Error opening file for writing"); return; }
	fprintf(fp, "Surname\tName\tMiddle name\tGroup\tBorn\tGender(0 - man, 1 - girl)\n"); // запис заголовка у файл
	List* temp = head;
	while (NULL != temp) {
		fprintf(fp, "%s\t%s\t%s\t%s\t%hd.%hd.%hd\t%d\n", temp->student.surname, temp->student.name, 
			temp->student.middle_name, temp->student.group, temp->student.birthday.year, temp->student.birthday.month, 
			temp->student.birthday.day, temp->student.gender);
		temp = temp->next;
	}
	fclose(fp);
	puts("\nData succesfully writed in file!");
}

void add_student(List** head) {
	Student st; // змінна структури, куди будуть записуватись значення, введені користувачем
	printf("Enter surname: "); 
	fgets(st.surname, sizeof(st.surname), stdin);
	st.surname[strcspn(st.surname, "\n")] = '\0'; // видаляю символ '\n' із введеного рядка
	printf("Enter name: ");
	fgets(st.name, sizeof(st.name), stdin);
	st.name[strcspn(st.name, "\n")] = '\0';
	printf("Enter middle name: ");
	fgets(st.middle_name, sizeof(st.middle_name), stdin);
	st.middle_name[strcspn(st.middle_name, "\n")] = '\0';
	printf("Enter group: ");
	fgets(st.group, sizeof(st.group), stdin);
	st.group[strcspn(st.group, "\n")] = '\0';
	printf("Enter full date of birth (year space month space day): ");
	while (1) {
		if (scanf_s("%hd%hd%hd", &st.birthday.year, &st.birthday.month, &st.birthday.day) != 3)
			printf("Entered incorrect value!\nTry again: ");
		else if (st.birthday.year < 1900 || st.birthday.month < 1 || st.birthday.month > 12 || st.birthday.day < 1
			|| st.birthday.day > 31)
			printf("Entered incorrect numbers!\nTry again: ");
		else break;
		cleanInputBuffer();
	} // перевірка на правильність і адекватність введених даних
	printf("Enter gender (0 - man, 1 - girl): ");
	do{
		if (scanf_s("%d", (int*)&st.gender) != 1) printf("Entered incorrect value!\nTry again: ");
		else if (st.gender != 0 && st.gender != 1) printf("Entered incorrect gender!\nTry again: ");
		cleanInputBuffer();
	} while (st.gender != 0 && st.gender != 1);
	List* newnode = (List*)malloc(sizeof(List)); // виділяю пам'ять для нового вузла списку
	if (NULL == newnode) { puts("Memory allocation error for adding :("); return; } // якщо пам'ять не виділелась
	newnode->student = st; newnode->next = 0;
	if (NULL == *head) *head = newnode;
	else {
		List* temp = *head;
		while (temp->next != NULL) temp = temp->next;
		temp->next = newnode;
	}
	puts("\nStudent succefully added in list\n");
}

void delete_student(List** head) { 
	char surname[100], name[100]; // рядки, які зберігатимуть введення даних про студента, якого потрібно видалити
	printf("\nEnter surname: ");
	fgets(surname, sizeof(surname), stdin);
	surname[strcspn(surname, "\n")] = '\0';
	printf("Enter name: ");
	fgets(name, sizeof(name), stdin);
	name[strcspn(name, "\n")] = '\0';
	lowercase(surname); lowercase(name); // переводжу введені дані про прізвище та ім'я в нижній регіср
	char sur2[100], nam2[100]; // рядки, куди будуть копіюватись прізвище та ім'я студента і в циклі переводитись в нижній регіср
	// основний алгоритм нижче
	List* curr = *head, *prev = 0;
	while (curr != NULL) {
		strcpy_s(sur2, sizeof(sur2), curr->student.surname); strcpy_s(nam2, sizeof(nam2), curr->student.name);
		lowercase(sur2); lowercase(nam2);
		if (strcmp(surname, sur2) == 0 && strcmp(name, nam2) == 0) 
			break; // виходимо з циклу, якщо студента знайдено
		prev = curr;
		curr = curr->next;
	}
	if (NULL == curr) { puts("\nStudent wasn\'t found!"); return; } 
	// якщо студента не знайдено в списку, тоді виходимо із функцій
	if (NULL == prev) *head = curr->next; // якщо потрібно видалити кореневий вузол
	else prev->next = curr->next;
	free(curr);
	puts("\nStudent successfully deleted\n");
}

void lowercase(char str[]) {
	for (short int i = 0; str[i] != '\0'; ++i) str[i] = tolower(str[i]);
	// ця функція працюватиме ЛИШЕ для латиниці, для кирилиці вона не працює
}
