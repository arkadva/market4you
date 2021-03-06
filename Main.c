#include "SuperMarket.h"

// Strings (Done)
void inputString(char** _str) {

	char str[100] = { NULL };

	scanf_s("%s", str, 100);
	*_str = malloc(strlen(str) * sizeof(char) + sizeof(char));
	strcpy(*_str, str);
}
void appendString(char** _str1, char* _str2) {

	char* str = malloc((strlen(*_str1) + strlen(_str2)) * sizeof(char) + sizeof(char));
	if (!str) exit(1);

	strcpy(str, *_str1);
	strcat(str, _str2);
	free(*_str1);
	*_str1 = str;
}
void appendStringComma(char** _str1, char* _str2) {

	appendString(_str1, _str2);
	appendString(_str1, ",");
}
void appendStringNewLine(char** _str1, char* _str2) {

	appendString(_str1, _str2);
	appendString(_str1, "\n");
}
char* copyString(char* _str) {

	char* str = malloc(sizeof(char) * strlen(_str) + sizeof(char));
	if (!str)
		exit(1);

	strcpy(str, _str);
	return str;
}
char* strToLower(char* _str) {

	char* str = copyString(_str);
	if (!str) exit(1);

	for (int i = 0; i < strlen(_str); i++)
		str[i] = tolower(str[i]);

	return str;
}

// Files (Done)
void checkFiles() {

	if (!doesFileExists(FILE_TICKET)) writeFile(FILE_TICKET, "");
	if (!doesFileExists(FILE_CUSTOMERS)) writeFile(FILE_CUSTOMERS, "");
	if (!doesFileExists(FILE_MANAGERS)) writeFile(FILE_MANAGERS, "");
	if (!doesFileExists(FILE_CATALOG)) writeFile(FILE_CATALOG, "");
	if (!doesFileExists(FILE_GLOBAL)) writeFile(FILE_GLOBAL, "0");
	if (!doesFileExists(FILE_ORDERS)) writeFile(FILE_ORDERS, "");
	if (!doesFileExists(FILE_TEMP)) writeFile(FILE_TEMP, "");
}
void checkFolder() {

	if (!doesFileExists(FOLDER_DATA)) createFolder(FOLDER_DATA);
	if (!doesFileExists(FOLDER_DATA_USERS)) createFolder(FOLDER_DATA_USERS);
	if (!doesFileExists(FOLDER_DATA_ORDERS)) createFolder(FOLDER_DATA_ORDERS);
	if (!doesFileExists(FOLDER_DATA_TICKETS)) createFolder(FOLDER_DATA_TICKETS);
}
void createFolder(char* dirname) {

	int check = mkdir(dirname);
	if (check) exit(1);
}
void writeFile(char* filename, char* content) {

	FILE* file = fopen(filename, "ab+");
	if (!file) exit(1);

	fprintf(file, "%s", content);
	fclose(file);
}
void writeUserType(Details* d, UserType type) {

	char output[200];

	if (type == customer) {

		sprintf(output, "%s,%s,%s,%.2f,%s\n", d->name, d->ID, d->password, 0.0, d->phone);
		writeFile(FILE_CUSTOMERS, output);
	}

	if (type == manager) {

		sprintf(output, "%s,%s,%s,%s\n", d->name, d->ID, d->password, d->phone);
		writeFile(FILE_MANAGERS, output);
	}
}
bool doesFileExists(char* filename) {

	struct stat buffer;
	return !stat(filename, &buffer);
}
Details* readUser(char* filename, UserType type) {

	Details details = { NULL, NULL, NULL, NULL, 0 };
	char nameString[100] = { NULL }, IdString[100] = { NULL }, passwordString[100] = { NULL }, phoneString[100] = { NULL };
	float supermarketPoints = 0;

	FILE* file = fopen(filename, "r");
	if (!file) exit(1);

	char buffer[500] = { NULL };
	while (fscanf(file, "%s", buffer) == 1) {

		if (type == customer) sscanf(buffer, "%[^,],%[^,],%[^,],%f,%[^,]", nameString, IdString, passwordString, &supermarketPoints, phoneString);
		if (type == manager)  sscanf(buffer, "%[^,],%[^,],%[^,],%[^,]", nameString, IdString, passwordString, phoneString);

		if (strcmp(IdString, Identity) == 0) {

			Details returnDetails = { copyString(nameString), copyString(IdString), copyString(passwordString), copyString(phoneString), supermarketPoints };
			fclose(file);
			return &returnDetails;
		}
	}

	fclose(file);
	return &details;
}
UserType findUserType() {

	Details* details = readUser(FILE_CUSTOMERS, customer);
	if (details->ID) return customer;

	details = readUser(FILE_MANAGERS, manager);
	if (details->ID) return manager;

	return none;
}

// Authentication (Done)
bool verifyName(Details* d) {

	char string[100] = { NULL };
	printf("Name --> ");
	scanf_s(" %[^\n]", string, 100);

	for (int i = 0; i < strlen(string); i++) {

		if (!((string[i] >= 'a' && string[i] <= 'z') || (string[i] >= 'A' && string[i] <= 'Z'))) {

			printf("Name Is Contain Only English Alphabet\n");
			d->name = NULL;
			return false;
		}
	}

	char* str = copyString(string);
	d->name = str;

	return true;
}
bool verifyPassword(Details* d) {

	char* str;
	int letters = 0, numbers = 0;

	printf("Password --> ");
	inputString(&str);

	if (strlen(str) < MIN_PASSWORD_LEN) {

		printf("Password Must Contain At 6 Characters\n");
		return false;
	}

	for (int i = 0; i < strlen(str); i++) {

		if (!(str[i] >= 'a' && str[i] <= 'z')) {

			if (!(str[i] >= 'A' && str[i] <= 'Z')) {

				if (!(str[i] >= '0' && str[i] <= '9'))
					return false;
				else numbers++;
			}
			else letters++;
		}
		else letters++;
	}

	if (!numbers) {
		printf("Password Must Contain At Least One Number\n");
		return false;
	}

	if (!letters) {
		printf("Password Must Contain At Least One letter\n");
		return false;
	}

	char* passwordVerification;
	printf("Password verification --> ");
	inputString(&passwordVerification);
	if (!strcmp(passwordVerification, str) == 0) {
		printf("Password verifciation failed.\n");
		return verifyPassword(d);
	}

	if (letters && numbers) {

		d->password = str;
		return true;
	}
	return false;
}
bool verifyId(Details* d) {

	char* str;

	printf("ID --> ");
	inputString(&str);

	Identity = str;

	long idVal = strtol(str, NULL, 10);
	int Digit1, Digit2, Digit3, Digit4, Digit5, Digit6, Digit7, Digit8, Digit9, Sum, Res;

	if ((idVal <= 999999999) && (idVal >= 100000000)) {

		Digit1 = (idVal % 1000000000) / 100000000;
		Digit2 = (idVal % 100000000) / 10000000;
		Digit3 = (idVal % 10000000) / 1000000;
		Digit4 = (idVal % 1000000) / 100000;
		Digit5 = (idVal % 100000) / 10000;
		Digit6 = (idVal % 10000) / 1000;
		Digit7 = (idVal % 1000) / 100;
		Digit8 = (idVal % 100) / 10;
		Digit9 = (idVal % 10);

		Digit1 = Digit1 * 1;
		Digit2 = Digit2 * 2;
		Digit3 = Digit3 * 1;
		Digit4 = Digit4 * 2;
		Digit5 = Digit5 * 1;
		Digit6 = Digit6 * 2;
		Digit7 = Digit7 * 1;
		Digit8 = Digit8 * 2;

		Digit2 = ((Digit2 / 10) + (Digit2 % 10));
		Digit4 = ((Digit4 / 10) + (Digit4 % 10));
		Digit6 = ((Digit6 / 10) + (Digit6 % 10));
		Digit8 = ((Digit8 / 10) + (Digit8 % 10));

		Sum = Digit1 + Digit2 + Digit3 + Digit4 + Digit5 + Digit6 + Digit7 + Digit8;

		Res = 10 - (Sum % 10);

		if (Res == Digit9) {

			if (findUserType(str)) {

				int interface = -1;
				printf("ID already exist in the system\n");

				if (!(interface > 0 && interface <= 2)) {

					printf("\nAvailable Actions\n'1' Wrong ID\t'2' Report for Identity steal\nInput --> ");
					scanf_s("%d", &interface);

					while (!(interface > 0 && interface <= 2)) {

						printf("Invalid input, Try again\nInput --> ");
						scanf_s("%d", &interface);
					}
				}

				switch (interface) {

				case 1:
					break;

				case 2:
					printf("We are sry to hear, Your report will be treated\nGoodbye\n");
					exit(1);
				}
				return false;
			}

			d->ID = str;
			return true;
		}

		else if (Res != Digit9)
			return false;
	}

	else
		return false;
}
bool verifyAge() {

	int age;

	printf("Age --> ");
	scanf_s("%d", &age);

	if (age < LEGAL_AGE) {

		printf("We are sry, The minimum age is 16\n");
		return false;
	}

	return true;
}
bool verifyPhone(Details* d) {

	char* str;

	printf("Phone --> ");
	inputString(&str);

	if (!str) exit(1);

	if (strlen(str) != PHONE_NUM_LEN) {

		printf("Phone Number Must Contain Ten Digits\n");
		return false;
	}

	for (int i = 0; i < strlen(str); i++) {

		if (!(str[i] >= '0' && str[i] <= '9')) {

			printf("Phone Number Contain Only Digits\n");
			return false;
		}
	}

	d->phone = str;
	return true;
}
bool verifyPayment(char* creditCard, char* cvv, int expirationMonth, int expirationYear) {

	if (expirationMonth <= 0 || expirationMonth > 12)
		return false;

	for (int i = 0; i < strlen(creditCard); i++)
		if (!(creditCard[i] >= '0' && creditCard[i] <= '9'))
			return false;

	for (int i = 0; i < strlen(cvv); i++)
		if (!(cvv[i] >= '0' && cvv[i] <= '9'))
			return false;

	Date date = getCurrentDate();
	if (date.year > expirationYear)
		return false;

	else if (date.year == expirationYear)
		return date.month <= expirationMonth;

	return true;
}
bool termsAndConditions() {

	char terms = { NULL };

	printf("Terms and Conditions\n\n1. The Site, including any content and / or service available through it, is provided to you 'As It Is'. Although the Company takes all efforts to present the Site or through it as accurate and reliable information as possible, the Company is not and will not be responsible, directly or indirectly, for the availability, veracity, reliability and/or accuracy of the content appearing on the Site, and reliance on any content displayed on or through the Site is at your full responsibility.\n\n2. You may use the Site and the content available through it for private and personal purposes only. The content of the Site does not grant you any rights other than those set forth in these Terms, which constitutes an agreement for all intents and purposes between you and the Company.\n\n3. The content of the Website may not be used as a basis for the purpose of making financial, legal, personal and/or other decisions on your part. Any action taken by you based on this Content is done at your sole discretion and at your own responsibility. Products, descriptions, design, colors or the appearance of products and services described or displayed on the site are for illustration purposes only, in order to simulate the user's desired product in the closest and best way. If you have identified a significant gap between the visibility of the actual requested product and the product appearing on the site, please contact us to correct it, at the company's sole discretion. In any case, the Company shall not be liable in connection with any product and/or recipe and/or recommendations detailed or displayed on the website and through it.\n\n");

	if (terms != 'Y' && terms != 'N') {

		printf("Do you agree to the terms and conditions (Y/N)\nInput --> ");
		(void)getchar();
		scanf_s("%c", &terms, 1);

		while (terms != 'Y' && terms != 'N') {

			printf("Invalid input, Try again\nInput --> ");
			(void)getchar();
			scanf_s("%c", &terms, 1);
		}
	}

	if (terms == 'Y') return true;

	else if (terms == 'N') return false;
}

//CustomerMenu(Done)
void customerMenu() {

	Cart c = { NULL , 0 };

	while (true) {

		int interface;
		printf("\nCustomer Actions\n'1' Store\t'2' Profile\t'3' View Cart\t'4' Finish Order\t'5' Submit Ticket\t'6' Exit\nInput --> ");
		scanf_s("%d", &interface);

		switch (interface) {

		case 1:
			customerShop(&c);
			break;

		case 2:
			profile();
			break;

		case 3:
			viewCart(&c);
			break;

		case 4:
			finishOrder(&c, Identity);
			break;

		case 5:
			openTicket();
			break;

		case 6:
			return false;
			break;
		}
	}
}
void customerShop(Cart* cart) {

	Product product;
	char* search;
	char* category = NULL;

	int interface = -1;
	if (!(interface <= 3 && interface >= 1)) {

		printf("\nSearch Product By\n'1' Name/Company\t'2' Category\t'3' Show All Products\nInput --> ");
		scanf_s("%d", &interface);

		while (!(interface <= 3 && interface >= 1)) {

			printf("Invalid input, Try again\nInput --> ");
			scanf_s("%d", &interface);
		}
	}

	if (interface == 1) {

		printf("\nName/Company --> ");
		inputString(&search);
		product = selectProduct(retrieveProducts(false, search, NULL));
	}

	else if (interface == 2)
		product = selectByCategory();

	else
		product = selectProduct(retrieveProducts(false, NULL, NULL));

	if (!product.name) {

		printf("No products founds.\n");
		return;
	}

	int selectedQuantity = -1;
	if (!(selectedQuantity > 0 && selectedQuantity <= product.quantity)) {

		printf("\nProduct Quantity --> ");
		scanf_s("%d", &selectedQuantity);

		while (!(selectedQuantity > 0 && selectedQuantity <= product.quantity)) {

			printf("Invalid input, Try again\nInput --> ");
			scanf_s("%d", &selectedQuantity);
		}
	}

	Product addedProduct = product;
	addedProduct.quantity = selectedQuantity;
	addToCart(cart, addedProduct);
	updateCatalog(&product, product.quantity - selectedQuantity);
}
void viewCart(Cart* cart) {

	printCart(cart);

	int interface = -1;
	if (!(interface > 0 && interface <= 2)) {

		printf("\n'1' Edit Cart\t'2' Back To Menu\nInput --> ");
		scanf_s("%d", &interface);

		while (!(interface > 0 && interface <= 2)) {

			printf("Invalid input, Try again\nInput --> ");
			scanf_s("%d", &interface);
		}
	}

	switch (interface) {

	case 1:
	{
		int optionA = 0;
		if (!(optionA - 1 < cart->itemsCount && optionA > 0)) {

			printf("Select Product --> ");
			scanf_s("%d", &optionA);

			while (!(optionA - 1 < cart->itemsCount && optionA > 0)) {

				printf("Invalid input, Try again\nInput --> ");
				scanf_s("%d", &optionA);

			}
		}

		int optionB = -1;
		if (!(optionB > 0 && optionB <= 3)) {

			printf("\n'1' Change Quantity\t'2' Remove Product\t'3' Exit\nInput --> ");
			scanf_s("%d", &optionB);

			while (!(optionB > 0 && optionB <= 3)) {

				printf("Invalid input, Try again\nInput --> ");
				scanf_s("%d", &optionB);

			}
		}

		if (optionB == 1) {

			optionB = -1;
			if (optionB < 0) {

				printf("New Quantity --> ");
				scanf_s("%d", &optionB);

				while (optionB < 0) {

					printf("Invalid input, Try again\nInput --> ");
					scanf_s("%d", &optionB);
				}
			}
			changeQuantity(cart, optionA - 1, optionB);
		}

		if (optionB == 2) {

			int optionA = 0;
			if (!(optionA - 1 < cart->itemsCount && optionA > 0)) {

				printf("Select Product --> ");
				scanf_s("%d", &optionA);

				while (!(optionA - 1 < cart->itemsCount && optionA > 0)) {

					printf("Invalid input, Try again\nInput --> ");
					scanf_s("%d", &optionA);
				}
			}
			removeFromCart(cart, optionA - 1);
		}
		break;
	}

	case 2:
		return;
	}
}
void printCart(Cart* cart) {

	printf("\n%-15s%-15s%-15s%-15s%-15s%-15s\n", "ID", "NAME", "COMPANY", "CATEGORY", "PRICE", "QUANTITY");

	float total = 0;

	for (int i = 0; i < cart->itemsCount; i++) {

		printf("%-15d%-15s%-15s%-15s%-15.2f%-15d\n",
			i + 1,
			cart->products[i].name,
			cart->products[i].company,
			cart->products[i].category,
			cart->products[i].price,
			cart->products[i].quantity);
		total += cart->products[i].price * cart->products[i].quantity;
	}
	printf("Total --> %.2f\n\n", total);
}
void openTicket() {

	char _Index[50] = { NULL }, _Report[500] = { NULL };
	int num = 0;

	FILE* User = fopen(FILE_TICKET, "ab+");
	if (!User) exit(1);

	printf("Report --> ");
	(void)getchar();
	scanf_s("%[^\n]", _Report, 500);

	char buffer[500] = { NULL };
	while (fgets(buffer, 500, User))
		num++;

	sprintf(_Index, "%d", num);
	fprintf(User, "%s,%s\n", _Index, _Report);
	fclose(User);
}
void addToCart(Cart* cart, Product product) {

	if (cart->itemsCount == 0) {

		cart->products = malloc(sizeof(Product));
		cart->products[0] = product;
	}

	else {
		Product* newProducts = malloc(sizeof(Product) * (cart->itemsCount + 1));
		for (int i = 0; i < cart->itemsCount; i++) newProducts[i] = cart->products[i];
		newProducts[cart->itemsCount] = product;
		cart->products = newProducts;
	}
	cart->itemsCount++;
}
void removeFromCart(Cart* cart, int index) {

	Product* newProduct = malloc(sizeof(Product) * (cart->itemsCount - 1));
	Product p = readProduct(FILE_CATALOG, cart->products[index].name, cart->products[index].company);

	updateCatalog(&p, p.quantity + cart->products[index].quantity);

	for (int i = 0; i < index; i++) {

		Product tempProduct = {
			copyString(cart->products[i].name),
			copyString(cart->products[i].company),
			copyString(cart->products[i].category),
			cart->products[i].price,
			cart->products[i].quantity
		};
		newProduct[i] = tempProduct;
	}

	for (int i = index + 1; i < index; i++) {

		Product tempProduct = {
		copyString(cart->products[i].name),
		copyString(cart->products[i].company),
		copyString(cart->products[i].category),
		cart->products[i].price,
		cart->products[i].quantity
		};
		newProduct[i - 1] = tempProduct;
	}
	cart->itemsCount--;
}
void changeQuantity(Cart* cart, int index, int newQuantity) {

	bool flag = false;
	Product p = readProduct(FILE_CATALOG, cart->products[index].name, cart->products[index].company);
	int availableQuantity = p.quantity;

	if (newQuantity > cart->products[index].quantity) {

		if (newQuantity - cart->products[index].quantity > availableQuantity)
			printf("Available Stock --> %d\n", availableQuantity);

		else {

			updateCatalog(&p, availableQuantity - (newQuantity - cart->products[index].quantity));
			cart->products[index].quantity = newQuantity;
		}
	}

	else {

		updateCatalog(&p, availableQuantity + (cart->products[index].quantity - newQuantity));
		cart->products[index].quantity = newQuantity;
	}
}
void writeOrder(Cart* cart, char* id) {

	char source[300] = { NULL };
	sprintf(source, "%s%s.csv", FOLDER_DATA_ORDERS, getNextOrderIdStr());

	FILE* file = fopen(source, "w");
	if (!file) exit(1);

	Date date = getCurrentDate();
	fprintf(file, "%d,%d,%d/%d/%d\n", getNextOrderId(), cart->itemsCount, date.day, date.month, date.year);

	float total = 0;

	for (int i = 0; i < cart->itemsCount; i++) {
		fprintf(file, "%s,%s,%s,%f,%d\n",
			cart->products[i].name,
			cart->products[i].company,
			cart->products[i].category,
			cart->products[i].price,
			cart->products[i].quantity);
		total += cart->products[i].price * cart->products[i].quantity;
	}

	fprintf(file, "%.2f\n", total);
	fclose(file);

	file = fopen(FILE_ORDERS, "ab+");
	if (!file) exit(1);

	fprintf(file, "%d,%s,%.2f,%d/%d/%d,WAITING\n", getNextOrderId(), id, total, date.day, date.month, date.year);
	fclose(file);

	appendOrderId();

	Details* user = readUser(FILE_CUSTOMERS, customer);
	float temp = user->points;

	int ans = 0;
	if (temp != 0) {

		printf("\nAvailable Market Points --> %.2f, Would You Like To Reedem em ? '1' Yes\t'2' No\nInput --> ", temp);
		scanf_s("%d", &ans);

		while (ans != 1 && ans != 2) {

			printf("Would You Like To Reedem Them In This Purchase? '1' Yes\t'2' No\nInput --> ");
			scanf_s("%d", &ans);
		}
		if (ans == 1) {
			temp = temp > total ? temp - total : 0;
			updatePoints(id, temp);
		}
	}
	printf("In This Purchase You've Earned %.2f Market Points (3%% Of Your Purchase Amount)\n", total * 0.03);
	updatePoints(id, temp + total * 0.03);
	int feedbak = 0;
	printf("Would you like to leave feedback on your shopping expirence?\n1.Yes\n2.No\n");
	while (!(feedbak == 1) && !(feedbak == 2))
	{
		scanf_s("%d", &feedbak);
		if (feedbak == 1)
			openTicket();
		else if (feedbak!=2)
			printf("Worng selection try again\n");
	}
	
}
void finishOrder(Cart* cart, char* id) {

	printCart(cart);

	int ans = 0;
	printf("Are You Sure You Would Like To Finish The Order?\n'1' Yes\n'2' No\nInput --> ");
	scanf_s("%d", &ans);

	while (ans != 1 && ans != 2) {
		printf("Incorrect answer, please try again.\n");
		printf("Are you sure you would like to finish the order? (Y/N) ");
		scanf_s("%d", &ans);
	}
	if (ans == 2)
		return;

	char creditCard[20] = { NULL }, cvv[20] = { NULL };
	int expirationMonth = 0, expirationYear = 0;

	printf("Credit Card --> ");
	scanf_s("%s", creditCard, 20);

	printf("CCV --> ");
	scanf_s("%s", cvv, 20);

	printf("Expiration Month (Numerical Value) --> ");
	scanf_s("%d", &expirationMonth);

	printf("Expiration Year (Numerical Value) --> ");
	scanf_s("%d", &expirationYear);

	while (!verifyPayment(creditCard, cvv, expirationMonth, expirationYear)) {

		printf("The entered card is invalid, please try again.\n");

		printf("Please enter your credit card number --> ");
		scanf_s("%s", creditCard, 20);

		printf("Please enter the CCV --> ");
		scanf_s("%s", cvv, 20);

		printf("Please enter expiration month (numerical value) --> ");
		scanf_s("%d", &expirationMonth);

		printf("Please enter expiration year --> ");
		scanf_s("%d", &expirationYear);
	}
	writeOrder(cart, id);
	cart->itemsCount = 0;
	free(cart->products);
}
void updatePoints(char* id, float newPoints) {

	if (!doesFileExists(FILE_CUSTOMERS)) {

		printf("Orders file missing..?");
		return;
	}

	FILE* file = fopen(FILE_CUSTOMERS, "r");
	if (!file) exit(1);

	int count = 0;
	char buffer[500] = { NULL };
	char* output = copyString("");

	while (fscanf(file, "%s", buffer) == 1) {
		char temp[500] = { NULL };
		char name[30] = { NULL }, customerId[40] = { NULL }, pass[30] = { NULL }, phone[30] = { NULL };
		float points = -1;

		sscanf(buffer, "%[^,],%[^,],%[^,],%f,%[^,]", name, customerId, pass, &points, phone);
		sprintf(temp, "%s,%s,%s,%f,%s\n", name, customerId, pass, strcmp(customerId, id) == 0 ? newPoints : points, phone);
		appendString(&output, temp);
	}

	fclose(file);
	file = fopen(FILE_CUSTOMERS, "w");
	if (!file) exit(1);

	fputs(output, file);
	fclose(file);
	free(output);
	printf("\nCurrent Market Points %.2f\n", newPoints);
}
Product selectByCategory() {

	printf("\n");
	Cart tempCart = retrieveProducts(false, NULL, NULL);
	int count = 0;
	char** categories = malloc(sizeof(char*) * tempCart.itemsCount);

	for (int i = 0; i < tempCart.itemsCount; i++) {

		bool flag = false;
		for (int j = 0; j < count; j++) {

			if (strcmp(tempCart.products[i].category, categories[j]) == 0) {

				flag = true;
				break;
			}
		}

		if (!flag) {

			categories[count] = malloc(sizeof(char) * strlen(tempCart.products[i].category) + sizeof(char));
			strcpy(categories[count], tempCart.products[i].category);
			printf("%d. %s\n", count + 1, categories[count]);
			count++;
		}
	}

	if (count == 0) {
		Product product = { NULL, NULL, NULL, 0, 0 };
		printf("No categories found..?\n");
		return product;
	}

	int ans = -1;
	while (!(ans > 0 && ans <= count)) {

		printf("Input No. --> ");
		scanf_s("%d", &ans);

		if (!(ans > 0 && ans <= count))
			printf("Invalid input, Try again\n");
	}
	ans--;
	tempCart = retrieveProducts(false, NULL, categories[ans]);
	return selectProduct(tempCart);
}

//ManagerMenu(Not ready)
void managerMenu() {

	while (true) {

		int interface = -1;

		if (!(interface > 0 && interface <= 6)) {

			printf("\nManager Actions\n'1' Store\t'2' Profile\t'3' View Orders \t'4' Store Profits \t'5' Reports\t'6' Exit\nInput --> ");
			scanf_s("%d", &interface);

			while (!(interface > 0 && interface <= 6)) {

				printf("Invalid input, Try again\nInput --> ");
				scanf_s("%d", &interface);
			}
		}

		switch (interface) {

		case 1:
			managerStoreActions();
			break;

		case 2:
			profile();
			break;

		case 3:
			showOrders(NULL);
			break;

		case 4:
			printRevenue();
			break;

		case 5:
			seeTickets();
			break;

		case 6:
			break;
		}
	}
}
void managerStoreActions() {

	int interface = -1;
	Product p;

	if (!(interface > 0 && interface <= 4)) {

		printf("\nStore Actions\n'1' Add Product\t'2' Delete Product\t'3' Update Product\t'4' Exit\nInput --> ");
		scanf_s("%d", &interface);

		while (!(interface > 0 && interface < 5)) {

			printf("Invalid input, Try again\nInput --> ");
			scanf_s("%d", &interface);
		}
	}

	switch (interface) {

	case 1:
		addToCatalog();
		break;

	case 2:
		p = selectProduct(retrieveProducts(true, NULL, NULL));
		deleteFromCatalog(&p);
		break;

	case 3:
		p = selectProduct(retrieveProducts(true, NULL, NULL));
		updateCatalog(&p, -1);
		break;

	case 4:
		break;
	}
}
void addToCatalog() {

	char* rProductName = NULL;
	while (!rProductName) {

		printf("\nName --> ");
		inputString(&rProductName);

		for (int i = 0; i < strlen(rProductName); i++) {

			if (!(rProductName[i] >= 'a' && rProductName[i] <= 'z' || rProductName[i] >= 'A' && rProductName[i] <= 'Z')) {

				rProductName = NULL;
				printf("Invalid input, Try again\n\n");
				break;
			}
		}
	}

	char* rProductCompany = NULL;
	while (!rProductCompany) {

		printf("Company --> ");
		inputString(&rProductCompany);

		if (doesProductExist(FILE_CATALOG, rProductName, rProductCompany)) {

			printf("Company Product already in stock\n");
			return;
		}

		for (int i = 0; i < strlen(rProductCompany); i++) {

			if (!(rProductCompany[i] >= 'a' && rProductCompany[i] <= 'z' || rProductCompany[i] >= 'A' && rProductCompany[i] <= 'Z')) {

				rProductCompany = NULL;
				printf("Invlaid input, Try again\n\n");
				break;
			}
		}
	}

	char* rProductCategory = NULL;
	while (!rProductCategory) {

		printf("Category --> ");
		inputString(&rProductCategory);

		for (int i = 0; i < strlen(rProductCategory); i++) {

			if (!(rProductCategory[i] >= 'a' && rProductCategory[i] <= 'z' || rProductCategory[i] >= 'A' && rProductCategory[i] <= 'Z')) {

				rProductCategory = NULL;
				printf("Invlaid input, Try again\n\n");
				break;
			}
		}
	}

	float rProductPrice = -1;
	if (rProductPrice < 0) {

		printf("Price --> ");
		scanf_s("%f", &rProductPrice);

		while (rProductPrice < 0) {

			printf("Invalid input, Try again\nInput --> ");
			scanf_s("%f", &rProductPrice);

		}
	}

	int rProductQuantity = -1;
	if (rProductQuantity < 0) {

		printf("Quantity --> ");
		scanf_s("%d", &rProductQuantity);

		while (rProductQuantity < 0) {

			printf("Invalid input, Try again\nInput --> ");
			scanf_s("%d", &rProductQuantity);
		}
	}

	char output[200] = { NULL };
	sprintf(output, "%s,%s,%s,%.2f,%d\n", rProductName, rProductCompany, rProductCategory, rProductPrice, rProductQuantity);
	writeFile(FILE_CATALOG, output);
}
void deleteFromCatalog(Product* p) {

	char _Name[100] = { NULL }, _Company[100] = { NULL }, _Category[100] = { NULL }, _Price[100] = { NULL }, _Quantity[100] = { NULL };

	FILE* Temp = fopen(FILE_TEMP, "w");
	if (!Temp) exit(1);

	FILE* CataLog = fopen(FILE_CATALOG, "r");
	if (!CataLog) exit(1);

	char buffer[500] = { NULL };
	while (fscanf(CataLog, "%s", buffer) == 1) {

		sscanf(buffer, "%[^,],%[^,],%[^,],%[^,],%[^,]", _Name, _Company, _Category, _Price, _Quantity);

		if (!(strcmp(p->name, _Name) == 0 && strcmp(p->company, _Company) == 0))
			fprintf(Temp, "%s,%s,%s,%s,%s\n", _Name, _Company, _Category, _Price, _Quantity);
	}

	fclose(CataLog);
	fclose(Temp);

	CataLog = fopen(FILE_CATALOG, "w");
	if (!CataLog) exit(1);

	Temp = fopen(FILE_TEMP, "r");
	if (!Temp) exit(1);

	while (fscanf(Temp, "%s", buffer) == 1) {

		sscanf(buffer, "%[^,],%[^,],%[^,],%[^,],%[^,]", _Name, _Company, _Category, _Price, _Quantity);
		fprintf(CataLog, "%s,%s,%s,%s,%s\n", _Name, _Company, _Category, _Price, _Quantity);
	}

	fclose(CataLog);
	fclose(Temp);
}
void updateCatalog(Product* p, int userQuantity) {

	char _Name[100] = { NULL }, _Company[100] = { NULL }, _Category[100] = { NULL }, _Price[100] = { NULL };
	char buffer[500] = { NULL }, newPrice[100] = { NULL }, newQuantity[100] = { NULL }, _Quantity[100] = { NULL };
	int interface = 0, updatedQuantity = -1;
	float updatedPrice = -1;

	FILE* Temp = fopen(FILE_TEMP, "w");
	if (!Temp) exit(1);

	FILE* CataLog = fopen(FILE_CATALOG, "r");
	if (!CataLog) exit(1);


	if (userQuantity == -1) {

		while (true) {

			printf("\nProduct Actions\n'1' Update Price\t'2' Update Quantity\nInput --> ");
			scanf_s("%d", &interface);

			if (interface == 1 || interface == 2)
				break;
		}
	}

	else
		interface = 2;

	while (fscanf(CataLog, "%s", buffer) == 1)
	{
		sscanf(buffer, "%[^,],%[^,],%[^,],%[^,],%[^,]", _Name, _Company, _Category, _Price, _Quantity);

		if (strcmp(p->name, _Name) == 0 && strcmp(p->company, _Company) == 0) {

			if (interface == 1) {

				if (updatedPrice < 0) {

					printf("Updated Price --> ");
					scanf_s("%f", &updatedPrice);

					while (updatedPrice < 0) {

						printf("Invalid input, Try again\nInput --> ");
						scanf_s("%f", &updatedPrice);
					}
				}

				sprintf(newPrice, "%.2f", updatedPrice);
				fprintf(Temp, "%s,%s,%s,%s,%s\n", _Name, _Company, _Category, newPrice, _Quantity);
			}

			if (interface == 2) {

				if (userQuantity == -1) {

					if (updatedQuantity < 0) {

						printf("Updated Quantity --> ");
						scanf_s("%d", &updatedQuantity);

						while (updatedQuantity < 0) {

							printf("Invalid input, Try again\nInput --> ");
							scanf_s("%d", &updatedQuantity);
						}
					}
				}

				sprintf(newQuantity, "%d", userQuantity == -1 ? updatedQuantity : userQuantity);
				fprintf(Temp, "%s,%s,%s,%s,%s\n", _Name, _Company, _Category, _Price, newQuantity);
			}
		}

		else
			fprintf(Temp, "%s,%s,%s,%s,%s\n", _Name, _Company, _Category, _Price, _Quantity);
	}

	fclose(CataLog);
	fclose(Temp);

	CataLog = fopen(FILE_CATALOG, "w");
	if (!CataLog) exit(1);

	Temp = fopen(FILE_TEMP, "r");
	if (!Temp) exit(1);

	while (fscanf(Temp, "%s", buffer) == 1) {

		sscanf(buffer, "%[^,],%[^,],%[^,],%[^,],%[^,]", _Name, _Company, _Category, _Price, _Quantity);
		fprintf(CataLog, "%s,%s,%s,%s,%s\n", _Name, _Company, _Category, _Price, _Quantity);
	}

	fclose(CataLog);
	fclose(Temp);
}
void seeTickets() {

	char buffer[500] = { NULL }, _Index[50] = { NULL }, _Report[400] = { NULL };

	FILE* User = fopen(FILE_TICKET, "r");
	if (!User) exit(1);

	while (fgets(buffer, 500, User))
		printf("%s", buffer);

	fclose(User);
}
void printRevenue() {

	int interface = -1, ordersCount = 0;
	Date tempDate = { 0,0,0 };
	float revenue = 0;
	char buffer[500] = { NULL };

	if (!(interface > 0 && interface <= 3)) {

		printf("\nRevenue Action\n'1' Last 30 Days\t'2' Last 7 Days\t'3' Current Day\nInput -- > ");
		scanf_s("%d", &interface);

		while (!(interface > 0 && interface <= 3)) {

			printf("Invalid input, Try again\nInput --> ");
			scanf_s("%d", &interface);
		}
	}

	FILE* file = fopen(FILE_ORDERS, "r");
	if (!file) exit(1);


	while (fscanf(file, "%s", buffer) == 1) {

		float price = 0;

		sscanf(buffer, "%*[^,],%*[^,],%f,%d/%d/%d,%*[^,]", &price, &tempDate.day, &tempDate.month, &tempDate.year);

		int dayDiff = calcDateDiff(tempDate);

		if (interface == 1) {

			if (dayDiff <= 30) {

				revenue += price;
				ordersCount++;
			}
		}

		else if (interface == 2) {

			if (dayDiff <= 7) {

				revenue += price;
				ordersCount++;
			}
		}

		else if (interface == 3) {

			if (dayDiff == 0) {

				revenue += price;
				ordersCount++;
			}
		}
	}
	fclose(file);
	printf("Asked Revenue --> %.2f In %d Orders\n", revenue, ordersCount);
}
void showOrders(char* id) {
	int* orders, * waitingOrders;
	int interface = -1, count = 0, waitingCount = 0;
	char buffer[500] = { NULL };

	if (id) interface = 1;

	if (!(interface > 0 && interface <= 2)) {

		printf("\nOrders Actions\n'1' Print All Orders\t'2' Confirm/Unconfirmed Orders\nInput --> ");
		scanf_s("%d", &interface);

		while (!(interface > 0 && interface <= 2)) {

			printf("Invalid input, Try again\nInput --> ");
			scanf_s("%d", &interface);
		}
	}

	FILE* file = fopen(FILE_ORDERS, "r");
	if (!file) exit(1);

	while (fscanf(file, "%s", buffer) == 1) {

		char status[30] = { NULL };
		char customerId[30] = { NULL };
		sscanf(buffer, "%*[^,],%[^,],%*[^,],%*[^,],%[^,]", customerId, status);

		if (id && strcmp(customerId, id) == 0)
			if (strcmp(status, "WAITING") == 0)
				waitingCount++;
			else count++;
		else {
			waitingCount++;
			count++;
		}
	}

	fclose(file);

	if (interface == 1) {

		file = fopen(FILE_ORDERS, "r");
		if (!file) exit(1);

		orders = malloc(sizeof(int) * count);

		printf("\n%-15s%-15s%-15s%-15s%-15s\n", "Order ID", "Customer ID", "Total", "Date", "Status");

		int i = 0;
		while (fscanf(file, "%s", buffer) == 1) {
			char customerId[30] = { NULL }, date[40] = { NULL }, status[30] = { NULL }, address[40] = { NULL };
			int orderId = -1;
			float price = -1;
			sscanf(buffer, "%d,%[^,],%f,%[^,],%[^,]", &orderId, customerId, &price, date, status);
			if (id && strcmp(customerId, id) == 0) {
				printf("%-15d%-15s%-15.2f%-15s%-15s\n", orderId, customerId, price, date, status);
				orders[i] = orderId;
				i++;
			}
			else {
				printf("%-15d%-15s%-15.2f%-15s%-15s\n", orderId, customerId, price, date, status);
				orders[i] = orderId;
				i++;
			}
		}

		bool flag = false;
		if (id) {
			int tempAns = 0;
			while (tempAns <= 0 || tempAns >= 3) {
				printf("Would you like to select an order\n1. Yes\n2. No\nEnter numerical value -- > ");
				scanf("%d", &tempAns);
				if (tempAns <= 0 || tempAns >= 3) printf("Incorrect selection.\n");
				if (tempAns == 2) return;
			}
		}
		while (!flag) {

			int ans = 0;

			printf("Select order (ID) -- > ");
			scanf_s("%d", &ans);

			for (int i = 0; i < count; i++) {
				if (orders[i] == ans) {
					printOrder(ans);
					free(orders);
					fclose(file);
					return;
				}
			}
			printf("Invalid selection, please try again.\n");
		}
	}

	else if (interface == 2) {

		file = fopen(FILE_ORDERS, "r");
		if (!file) exit(1);

		waitingOrders = malloc(sizeof(int) * waitingCount);

		printf("\n%-15s%-15s%-15s%-15s%-15s\n", "Order ID", "Customer ID", "Total", "Date", "Status");

		int i = 0;

		while (fscanf(file, "%s", buffer) == 1) {
			char customerId[30] = { NULL }, date[40] = { NULL }, status[30] = { NULL };
			int orderId = -1;
			float price = -1;

			sscanf(buffer, "%d,%[^,],%f,%[^,],%[^,]", &orderId, customerId, &price, date, status);
			if (strcmp(status, "WAITING") == 0) {
				printf("%-15d%-15s%-15.2f%-15s%-15s\n", orderId, customerId, price, date, status);
				waitingOrders[i] = orderId;
				i++;
			}
		}
		bool flag = false;

		while (!flag) {
			int ans;
			printf("Select order (ID) -- > ");
			scanf_s("%d", &ans);
			for (int i = 0; i < waitingCount; i++) {
				if (waitingOrders[i] == ans) {
					printOrder(ans);
					changeOrderStatus(ans);
					free(waitingOrders);
					fclose(file);
					return;
				}
			}
			printf("Invalid selection, please try again.\n");
		}
	}
}
void printOrder(int orderId) {

	char source[300] = { NULL }, str[20] = { NULL }, name[100] = { NULL }, company[100] = { NULL }, category[100] = { NULL };
	int count = 0, quantity = -1;
	float price = -1;

	sprintf(source, "%s%d.csv", FOLDER_DATA_ORDERS, orderId);

	if (!doesFileExists(source)) {
		printf("Order doesn't exist.");
		return;
	}

	FILE* file = fopen(source, "r");
	if (!file) exit(1);

	fscanf(file, "%*d,%d,%s", &count, str);

	printf("\nOrder ID --> %d from %s\n", orderId, str);
	printf("%-14s%-15s%-15s%-15s%s", "NAME", "COMPANY", "CATEGORY", "PRICE", "QUANTITY");


	for (int i = 0; i < count; i++) {
		fscanf(file, "%[^,],%[^,],%[^,],%f,%d", name, company, category, &price, &quantity);
		printf("%-15s%-15s%-15s%-15.2f%-15d",
			name,
			company,
			category,
			price,
			quantity);
	}
	fscanf(file, "%f", &price);
	printf("\nTotal --> %.2f\n", price);
	fclose(file);
}
void changeOrderStatus(int id) {

	int count = 0;
	char buffer[500] = { NULL };
	char* output = copyString("");

	if (!doesFileExists(FILE_ORDERS)) {
		printf("Orders file missing..?");
		return;
	}

	FILE* file = fopen(FILE_ORDERS, "r");
	if (!file) exit(1);

	while (fscanf(file, "%s", buffer) == 1) {
		char temp[500] = { NULL }, customerId[30] = { NULL }, date[40] = { NULL }, status[30] = { NULL };
		int orderId = -1;
		float price = -1;

		sscanf(buffer, "%d,%[^,],%f,%[^,],%[^,]", &orderId, customerId, &price, date, status);
		sprintf(temp, "%d,%s,%f,%s,%s\n", orderId, customerId, price, date, id == orderId ? "CONFIRMED" : status);
		appendString(&output, temp);
	}

	fclose(file);

	file = fopen(FILE_ORDERS, "w");
	if (!file) exit(1);

	fputs(output, file);
	fclose(file);
	free(output);
	printf("Order Has Been Approved\n");
}
int calcDateDiff(Date d2) {

	Date d1 = getCurrentDate();
	int daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int count1 = d1.year * 365 + d1.day;
	int count2 = d2.year * 365 + d2.day;

	for (int i = 0; i < d1.month; i++)
		count1 += daysInMonth[i];

	for (int i = 0; i < d2.month; i++)
		count2 += daysInMonth[i];

	return count1 - count2;
}
Date getCurrentDate() {

	Date date = { 0,0,0 };
	struct tm* tm;
	time_t t;
	char str_date[100] = { NULL };
	t = time(NULL);
	tm = localtime(&t);
	strftime(str_date, sizeof(str_date), "%d %m %Y", tm);
	sscanf(str_date, "%d %d %d", &date.day, &date.month, &date.year);
	return date;
}

//Manager + Customer (Status)
void registerUserType(UserType type) {

	Details d;

	printf("\nRegister Stage\n");

	while (!verifyName(&d))
		printf("Invalid, Try again\n\n");

	while (!verifyPassword(&d))
		printf("Invalid, Try again\n\n");

	while (!verifyId(&d))
		printf("Invalid, Try again!\n\n");

	if (type == customer)
		if (!verifyAge())
			return;

	while (!verifyPhone(&d))
		printf("Invalid, Try again\n\n");

	if (!termsAndConditions())
		return;

	writeUserType(&d, type);
}
void welcomeScreen() {

	int interface = -1;

	if (!(interface > 0 && interface <= 3)) {

		printf("Welcome To HomeScreen\n'1' Register\t'2' Login\t'3' Exit\nInput --> ");
		scanf_s("%d", &interface);

		while (!(interface > 0 && interface <= 3)) {

			printf("Invalid input, Try again\nInput --> ");
			scanf_s("%d", &interface);
		}

	}

	switch (interface) {

	case 1:
		registerStage();
		break;

	case 2:
		loginUser();
		break;

	case 3:
		break;
	}
}
void registerStage() {

	int interface = -1, managerCode = 0;

	if (!(interface > 0 && interface <= 3)) {

		printf("\nRegister as\n'1' Customer\t'2' Manager\t'3' Exit\nInput --> ");
		scanf_s("%d", &interface);

		while (!(interface > 0 && interface <= 3)) {

			printf("Invalid input, Try again\nInput --> ");
			scanf_s("%d", &interface);
		}
	}

	switch (interface) {

	case 1:
		registerUserType(customer);
		break;

	case 2:
		printf("Input manager code --> ");
		scanf_s("%d", &managerCode);

		if (MANAGER_CODE == managerCode)
			registerUserType(manager);

		else
			printf("Wrong Code\n");
		break;

	case 3:
		break;
	}
}
void loginUser() {

	char* userId, * userPassword;

	printf("\nLogin Stage\nID --> ");
	inputString(&userId);

	Identity = userId;

	UserType type = findUserType();

	if (!type) {

		printf("ID Does Not Exist\n");
		return none;
	}

	printf("Password --> ");
	inputString(&userPassword);

	Details* user = readUser(type == customer ? FILE_CUSTOMERS : FILE_MANAGERS, type);

	if (strcmp(readUser(type == customer ? FILE_CUSTOMERS : FILE_MANAGERS, readPassword)->password, userPassword) == 0) {

		printf("You've Logged As A ");

		if (type == customer) {

			printf("Customer\n");
			customerMenu();
		}

		if (type == manager) {

			printf("Manager.\n");
			managerMenu();
		}
	}

	else
		printf("Invalid Password\n");
}
void profile() {
	printProfile();
	int interface = -1;

	if (!(interface > 0 && interface <= 2)) {

		printf("\nProfile Actions\n'1' Update Profile\t'2' Exit\nInput --> ");
		scanf_s("%d", &interface);

		while (!(interface > 0 && interface <= 2)) {

			printf("Invalid input, Try again\nInput --> ");
			scanf_s("%d", &interface);
		}
	}

	switch (interface) {

	case 1:
		updateProfile();
		break;

	case 2:
		break;
	}
}
void printProfile() {

	Details* details = readUser(findUserType(Identity) == customer ? FILE_CUSTOMERS : FILE_MANAGERS, findUserType(Identity));
	char n[100] = { NULL }, i[100] = { NULL }, p[100] = { NULL }, ph[100] = { NULL };

	strcpy(n, details->name);
	strcpy(i, details->ID);
	strcpy(p, details->password);
	strcpy(ph, details->phone);
	float po = details->points;

	printf("\nName --> %s\n", n);
	printf("ID --> %s\n", i);
	printf("Password --> %s\n", p);
	printf("Phone --> %s\n", ph);

	if (findUserType(Identity) == customer)
		printf("Supermarket Points --> %.2f\n", po);
	showOrders(Identity);
}
void updateProfile() {

	int interface = -1;
	char _Name[100] = { NULL }, _Id[100] = { NULL }, _Password[100] = { NULL }, _Phone[100] = { NULL }, _Points[100] = { NULL }, buffer[500] = { NULL };
	Details d;

	FILE* Temp = fopen(FILE_TEMP, "w");
	if (!Temp) exit(1);

	FILE* User = fopen(findUserType(Identity) == customer ? FILE_CUSTOMERS : FILE_MANAGERS, "r");
	if (!User) exit(1);

	if (findUserType(Identity) == customer) {

		while (fscanf(User, "%s", buffer) == 1) {

			sscanf(buffer, "%[^,],%[^,],%[^,],%[^,],%[^,]", _Name, _Id, _Password, _Points, _Phone);

			if ((strcmp(Identity, _Id)) == 0) {

				if (!(interface > 0 && interface <= 4)) {
					printf("\nChange Options\n'1' Name\t'2' ID\t'3' Password\t'4' Phone\nInput --> ");
					scanf_s("%d", &interface);

					while (!(interface > 0 && interface <= 4)) {

						printf("Invalid, Try again\nInput --> ");
						scanf_s("%d", &interface);
					}
				}

				switch (interface) {

				case 1:
					while (true) {

						if (verifyName(&d))
							break;
					}

					fprintf(Temp, "%s,%s,%s,%s,%s\n", d.name, _Id, _Password, _Points, _Phone);
					break;

				case 2:
					while (true) {

						if (verifyId(&d))
							break;

					}

					fprintf(Temp, "%s,%s,%s,%s,%s\n", _Name, d.ID, _Password, _Points, _Phone);
					break;

				case 3:
					while (true) {

						if (verifyPassword(&d))
							break;
					}
					fprintf(Temp, "%s,%s,%s,%s,%s\n", _Name, _Id, d.password, _Points, _Phone);
					break;

				case 4:
					while (true) {

						if (verifyPhone(&d))
							break;
					}

					fprintf(Temp, "%s,%s,%s,%s,%s\n", _Name, _Id, _Password, _Points, d.phone);
					break;
				}
			}

			else
				fprintf(Temp, "%s,%s,%s,%s,%s\n", _Name, _Id, _Password, _Points, _Phone);
		}

		fclose(Temp);
		fclose(User);

		User = fopen(FILE_CUSTOMERS, "w");
		if (!User) exit(1);

		Temp = fopen(FILE_TEMP, "r");
		if (!Temp) exit(1);

		while (fscanf(Temp, "%s", buffer) == 1) {

			sscanf(buffer, "%[^,],%[^,],%[^,],%[^,],%[^,]", _Name, _Id, _Password, _Points, _Phone);
			fprintf(User, "%s,%s,%s,%s,%s\n", _Name, _Id, _Password, _Points, _Phone);
		}
		fclose(Temp);
		fclose(User);
	}


	else if (findUserType(Identity) == manager) {

		while (fscanf(User, "%s", buffer) == 1) {

			sscanf(buffer, "%[^,],%[^,],%[^,],%[^,]", _Name, _Id, _Password, _Phone);

			if ((strcmp(Identity, _Id)) == 0) {

				if (!(interface > 0 && interface <= 4)) {
					printf("\nChange Options\n'1' Name\t'2' ID\t'3' Password\t'4' Phone\nInput --> ");
					scanf_s("%d", &interface);

					while (!(interface > 0 && interface < 5)) {

						printf("Invalid, Try again\nInput --> ");
						scanf_s("%d", &interface);
					}
				}

				switch (interface) {

				case 1:
					while (true) {

						if (verifyName(&d))
							break;
					}

					fprintf(Temp, "%s,%s,%s,%s\n", d.name, _Id, _Password, _Phone);
					break;

				case 2:
					while (true) {

						if (verifyId(&d))
							break;

					}

					fprintf(Temp, "%s,%s,%s,%s\n", _Name, d.ID, _Password, _Phone);
					break;

				case 3:
					while (true) {

						if (verifyPassword(&d))
							break;
					}
					fprintf(Temp, "%s,%s,%s,%s\n", _Name, _Id, d.password, _Phone);
					break;

				case 4:
					while (true) {

						if (verifyPhone(&d))
							break;
					}

					fprintf(Temp, "%s,%s,%s,%s\n", _Name, _Id, _Password, d.phone);
					break;
				}
			}

			else
				fprintf(Temp, "%s,%s,%s,%s\n", _Name, _Id, _Password, _Phone);
		}

		fclose(Temp);
		fclose(User);

		User = fopen(FILE_MANAGERS, "w");
		if (!User) exit(1);

		Temp = fopen(FILE_TEMP, "r");
		if (!Temp) exit(1);

		while (fscanf(Temp, "%s", buffer) == 1) {

			sscanf(buffer, "%[^,],%[^,],%[^,],%[^,]", _Name, _Id, _Password, _Phone);
			fprintf(User, "%s,%s,%s,%s\n", _Name, _Id, _Password, _Phone);
		}
		fclose(Temp);
		fclose(User);
	}
}
bool doesProductExist(char* filename, char* _productName, char* _company) {

	Product tempProduct = { NULL, NULL, NULL, 0, 0 };
	char name[100] = { NULL }, company[100] = { NULL }, buffer[500] = { NULL };

	FILE* file = fopen(filename, "r");
	if (!file) exit(1);

	while (fscanf(file, "%s", buffer) == 1) {

		sscanf(buffer, "%[^,],%[^,],%*[^,],%*d,%*f", name, company);

		if (strcmp(_productName, name) == 0 && strcmp(_company, company) == 0) {

			fclose(file);
			return true;
		}
	}

	fclose(file);
	return false;
}
Product selectProduct(Cart cart) {

	Product product = { NULL, NULL, NULL, 0, 0 };
	int i = 1, res = 0;
	char buffer[500] = { NULL };

	printf("\n%-15s%-15s%-15s%-15s%-15s%-15s\n", "ID", "NAME", "COMPANY", "CATEGORY", "PRICE", "QUANTITY");

	for (i = 0; i < cart.itemsCount; i++) {
		printf("%-15d%-15s%-15s%-15s%-15.2f%-15d\n",
			i + 1,
			cart.products[i].name,
			cart.products[i].company,
			cart.products[i].category,
			cart.products[i].price,
			cart.products[i].quantity);
	}

	printf("\nAvailable Action\n'0' Sort By Ascending Price\tElse Select Product\nInput --> ");
	scanf_s("%d", &res);

sort:
	if (res == 0) {
		for (int p = 0; p < cart.itemsCount; p++) {
			for (int j = p + 1; j < cart.itemsCount; j++) {
				if (cart.products[p].price > cart.products[j].price) {
					Product tempPrd = cart.products[p];
					cart.products[p] = cart.products[j];
					cart.products[j] = tempPrd;
				}
			}
		}

		printf("\n%-15s%-15s%-15s%-15s%-15s%-15s\n", "ID", "NAME", "COMPANY", "CATEGORY", "PRICE", "QUANTITY");
		for (int i = 0; i < cart.itemsCount; i++) {
			printf("%-15d%-15s%-15s%-15s%-15.2f%-15d\n",
				i + 1,
				cart.products[i].name,
				cart.products[i].company,
				cart.products[i].category,
				cart.products[i].price,
				cart.products[i].quantity);
		}

		printf("Select Product --> ");
		scanf_s("%d", &res);
		while (!(res - 1 >= 0 && res - 1 < i)) {
			printf("Invalid selection, please try again.\n");
			printf("Select product --> ");
			scanf_s("%d", &res);
		}
		return cart.products[res - 1];
	}
	else {
		while (!(res - 1 >= 0 && res - 1 < i)) {
			printf("Invalid selection, please try again.\n");
			printf("Select product --> ");
			scanf_s("%d", &res);
			if (res == 0) goto sort;
		}
		return cart.products[res - 1];
	}
	return product;
}
Product readProduct(char* filename, char* _productName, char* _company) {

	Product tempProduct = { NULL, NULL, NULL, 0, 0 };
	char name[100] = { NULL }, company[100] = { NULL }, category[100] = { NULL }, buffer[500] = { NULL };
	int quantity = 0;
	float price = 0;

	FILE* file = fopen(filename, "r");
	if (!file) exit(1);

	while (fscanf(file, "%s", buffer) == 1) {
		sscanf(buffer, "%[^,],%[^,],%[^,],%f, %d", name, company, category, &price, &quantity);

		if (strcmp(_productName, name) == 0 && strcmp(_company, company) == 0) {

			Product anotherTempProduct = { copyString(name), copyString(company), copyString(category), quantity, price };
			return anotherTempProduct;
		}
	}
	return tempProduct;
}
Cart retrieveProducts(bool returnAll, char* search, char* searchCategory) {

	int count = 0;
	char buffer[500] = { NULL };

	FILE* file = fopen(FILE_CATALOG, "r");
	if (!file) exit(1);

	while (fscanf(file, "%s", buffer) == 1) {

		char name[40] = { NULL }, company[40] = { NULL }, category[40] = { NULL };
		int quantity = -1;

		sscanf(buffer, "%[^,],%[^,],%[^,],%*f,%d", name, company, category, &quantity);

		if (search) {

			if (strcmp(strToLower(name), strToLower(search)) == 0 || strcmp(strToLower(company), strToLower(search)) == 0) {

				if (quantity > 0 || returnAll)
					count++;
			}
		}

		else if (searchCategory) {

			if (strcmp(category, searchCategory) == 0) {
				if (quantity > 0 || returnAll)
					count++;
			}
		}

		else if (quantity > 0 || returnAll)
			count++;
	}

	char name[100], company[100], category[100];
	int quantity = -1;
	float price = -1;
	int ite = 0;
	Cart cart = { 0, NULL };
	cart.products = malloc(sizeof(Product) * count);
	cart.itemsCount = count;
	fclose(file);
	file = fopen(FILE_CATALOG, "r");
	while (fscanf(file, "%s", buffer) == 1) {
		sscanf(buffer, "%[^,],%[^,],%[^,],%f,%d", name, company, category, &price, &quantity);
		if (search) {
			if (strcmp(strToLower(name), strToLower(search)) == 0 || strcmp(strToLower(company), strToLower(search)) == 0) {
				if (quantity > 0 || returnAll) {
					Product tempProduct = { copyString(name), copyString(company), copyString(category), quantity, price };
					cart.products[ite] = tempProduct;
					ite++;
				}
			}
		}
		else if (searchCategory) {
			if (strcmp(category, searchCategory) == 0) {
				if (quantity > 0 || returnAll) {
					Product tempProduct = { copyString(name), copyString(company), copyString(category), quantity, price };
					cart.products[ite] = tempProduct;
					ite++;
				}
			}
		}
		else if (quantity > 0 || returnAll) {
			Product tempProduct = { copyString(name), copyString(company), copyString(category), quantity, price };
			cart.products[ite] = tempProduct;
			ite++;
		}
	}
	fclose(file);
	return cart;
}

//Place in the right subject
int getNextOrderId() {

	int next = 0;

	FILE* file = fopen(FILE_GLOBAL, "r");
	if (!file) exit(1);

	fscanf(file, "%d", &next);
	fclose(file);
	return next;
}
char* getNextOrderIdStr() {

	char* str = malloc(sizeof(char) * 100);

	FILE* file = fopen(FILE_GLOBAL, "r");
	if (!file) exit(1);

	fscanf(file, "%s", str);
	fclose(file);
	return str;
}
void appendOrderId() {

	int next = 0;

	FILE* file = fopen(FILE_GLOBAL, "r");
	if (!file) exit(1);

	fscanf(file, "%d", &next);
	fclose(file);
	file = fopen(FILE_GLOBAL, "w");
	fprintf(file, "%d", ++next);
	fclose(file);
}
int main() {

	checkFolder();
	checkFiles();
	while (true)
		welcomeScreen();

	return 0;
}