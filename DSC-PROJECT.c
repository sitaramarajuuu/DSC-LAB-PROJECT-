/*
 * =============================================
 *        FEE MANAGEMENT SYSTEM
 *    Track paid/unpaid fees per student
 * =============================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STUDENTS 100
#define FILE_NAME    "fees.dat"

/* ---- Student Record Structure ---- */
typedef struct {
    int   id;
    char  name[50];
    char  course[30];
    char  contact[15];
    float totalFee;
    float paidAmount;
} Student;

/* ---- Global Data ---- */
Student students[MAX_STUDENTS];
int     studentCount = 0;

/* ===================== HELPER FUNCTIONS ===================== */

float getPending(Student s) {
    return s.totalFee - s.paidAmount;
}

const char* getStatus(Student s) {
    float pending = getPending(s);
    if (pending <= 0)           return "FULLY PAID";
    if (s.paidAmount == 0)      return "NOT PAID";
    return "PARTIAL";
}

/* Find a student by ID; returns index or -1 if not found */
int findById(int id) {
    for (int i = 0; i < studentCount; i++)
        if (students[i].id == id) return i;
    return -1;
}

/* ===================== FILE OPERATIONS ===================== */

void saveToFile() {
    FILE *fp = fopen(FILE_NAME, "wb");
    if (!fp) { printf("  [!] Error saving file!\n"); return; }
    fwrite(&studentCount, sizeof(int), 1, fp);
    fwrite(students, sizeof(Student), studentCount, fp);
    fclose(fp);
}

void loadFromFile() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) return;
    fread(&studentCount, sizeof(int), 1, fp);
    fread(students, sizeof(Student), studentCount, fp);
    fclose(fp);
    printf("  [✓] %d student record(s) loaded.\n", studentCount);
}

/* ===================== CORE FEATURES ===================== */

/* 1. Add a new student */
void addStudent() {
    if (studentCount >= MAX_STUDENTS) {
        printf("  [!] Maximum student limit reached!\n"); return;
    }

    Student s;
    s.id = (studentCount == 0) ? 1 : students[studentCount - 1].id + 1;
    s.paidAmount = 0;

    printf("\n--- Add New Student ---\n");
    printf("Name    : "); scanf(" %[^\n]", s.name);
    printf("Course  : "); scanf(" %[^\n]", s.course);
    printf("Contact : "); scanf(" %[^\n]", s.contact);
    printf("Total Fee (Rs): "); scanf("%f", &s.totalFee);

    if (s.totalFee <= 0) { printf("  [!] Fee must be positive!\n"); return; }

    students[studentCount++] = s;
    saveToFile();

    printf("\n  [✓] Student added successfully!\n");
    printf("  Student ID: %d | Name: %s | Total Fee: Rs %.2f\n", s.id, s.name, s.totalFee);
}

/* 2. Record a fee payment */
void payFee() {
    int   id;
    float amount;

    printf("\n--- Record Fee Payment ---\n");
    printf("Enter Student ID: ");
    scanf("%d", &id);

    int idx = findById(id);
    if (idx == -1) { printf("  [!] Student ID %d not found!\n", id); return; }

    Student *s = &students[idx];
    float pending = getPending(*s);

    if (pending <= 0) {
        printf("  [✓] %s has already paid all fees!\n", s->name); return;
    }

    printf("\n  Student : %s\n", s->name);
    printf("  Total   : Rs %.2f\n", s->totalFee);
    printf("  Paid    : Rs %.2f\n", s->paidAmount);
    printf("  Pending : Rs %.2f\n", pending);
    printf("\nEnter amount to pay (Rs): ");
    scanf("%f", &amount);

    if (amount <= 0) { printf("  [!] Amount must be positive!\n"); return; }
    if (amount > pending) {
        printf("  [!] Amount (Rs %.2f) exceeds pending (Rs %.2f)!\n", amount, pending);
        return;
    }

    s->paidAmount += amount;
    saveToFile();

    printf("\n  [✓] Payment of Rs %.2f recorded for %s.\n", amount, s->name);
    printf("  Remaining Pending: Rs %.2f\n", getPending(*s));
    printf("  Status: %s\n", getStatus(*s));
}

/* 3. View all students */
void viewAll() {
    if (studentCount == 0) { printf("  [!] No student records found!\n"); return; }

    printf("\n");
    printf("  %-5s %-20s %-15s %12s %12s %12s   %-12s\n",
           "ID", "Name", "Course", "Total(Rs)", "Paid(Rs)", "Pending(Rs)", "Status");
    printf("  -----------------------------------------------");
    printf("---------------------------------------------------\n");

    for (int i = 0; i < studentCount; i++) {
        printf("  %-5d %-20s %-15s %12.2f %12.2f %12.2f   %-12s\n",
               students[i].id,
               students[i].name,
               students[i].course,
               students[i].totalFee,
               students[i].paidAmount,
               getPending(students[i]),
               getStatus(students[i]));
    }

    printf("  -----------------------------------------------");
    printf("---------------------------------------------------\n");
    printf("  Total Records: %d\n", studentCount);
}

/* 4. Search a student by ID */
void searchStudent() {
    int id;
    printf("\nEnter Student ID to search: ");
    scanf("%d", &id);

    int idx = findById(id);
    if (idx == -1) { printf("  [!] Student not found!\n"); return; }

    Student s = students[idx];
    printf("\n  ==============================\n");
    printf("   STUDENT DETAILS\n");
    printf("  ==============================\n");
    printf("  ID       : %d\n",   s.id);
    printf("  Name     : %s\n",   s.name);
    printf("  Course   : %s\n",   s.course);
    printf("  Contact  : %s\n",   s.contact);
    printf("  Total Fee: Rs %.2f\n", s.totalFee);
    printf("  Paid     : Rs %.2f\n", s.paidAmount);
    printf("  Pending  : Rs %.2f\n", getPending(s));
    printf("  Status   : %s\n",   getStatus(s));
    printf("  ==============================\n");
}

/* 5. Show only students with pending fees */
void viewPendingOnly() {
    printf("\n--- Students with Pending Fees ---\n");
    int found = 0;

    printf("  %-5s %-20s %-15s %12s   %-12s\n",
           "ID", "Name", "Course", "Pending(Rs)", "Status");
    printf("  ----------------------------------------------------------------\n");

    for (int i = 0; i < studentCount; i++) {
        if (getPending(students[i]) > 0) {
            printf("  %-5d %-20s %-15s %12.2f   %-12s\n",
                   students[i].id,
                   students[i].name,
                   students[i].course,
                   getPending(students[i]),
                   getStatus(students[i]));
            found++;
        }
    }

    if (!found) printf("  [✓] No pending fees! All students have paid.\n");
    else        printf("  Total Students with Pending Fees: %d\n", found);
}

/* 6. Show only students who have fully paid */
void viewPaidOnly() {
    printf("\n--- Fully Paid Students ---\n");
    int found = 0;

    printf("  %-5s %-20s %-15s %12s\n", "ID", "Name", "Course", "Total Paid(Rs)");
    printf("  -----------------------------------------------------------\n");

    for (int i = 0; i < studentCount; i++) {
        if (getPending(students[i]) <= 0) {
            printf("  %-5d %-20s %-15s %12.2f\n",
                   students[i].id,
                   students[i].name,
                   students[i].course,
                   students[i].paidAmount);
            found++;
        }
    }

    if (!found) printf("  No fully paid students yet.\n");
    else        printf("  Total Fully Paid: %d\n", found);
}

/* 7. Overall fee summary / report */
void feeSummary() {
    float totalFees      = 0;
    float totalCollected = 0;
    float totalPending   = 0;
    int   fullyPaid      = 0;
    int   partial        = 0;
    int   notPaid        = 0;

    for (int i = 0; i < studentCount; i++) {
        totalFees      += students[i].totalFee;
        totalCollected += students[i].paidAmount;
        totalPending   += getPending(students[i]);

        if (getPending(students[i]) <= 0)     fullyPaid++;
        else if (students[i].paidAmount == 0) notPaid++;
        else                                  partial++;
    }

    printf("\n");
    printf("  ==========================================\n");
    printf("            FEE COLLECTION SUMMARY\n");
    printf("  ==========================================\n");
    printf("  Total Students     : %d\n",     studentCount);
    printf("  ------------------------------------------\n");
    printf("  Total Fee Due      : Rs %.2f\n", totalFees);
    printf("  Total Collected    : Rs %.2f\n", totalCollected);
    printf("  Total Pending      : Rs %.2f\n", totalPending);
    printf("  ------------------------------------------\n");
    printf("  Fully Paid         : %d student(s)\n", fullyPaid);
    printf("  Partial Payment    : %d student(s)\n", partial);
    printf("  Not Paid at All    : %d student(s)\n", notPaid);
    printf("  ==========================================\n");
    if (totalFees > 0)
        printf("  Collection Rate    : %.1f%%\n",
               (totalCollected / totalFees) * 100);
    printf("  ==========================================\n");
}

/* 8. Delete a student record */
void deleteStudent() {
    int id;
    char confirm;

    printf("\nEnter Student ID to delete: ");
    scanf("%d", &id);

    int idx = findById(id);
    if (idx == -1) { printf("  [!] Student not found!\n"); return; }

    printf("  Are you sure you want to delete '%s'? (y/n): ", students[idx].name);
    scanf(" %c", &confirm);

    if (confirm != 'y' && confirm != 'Y') { printf("  Cancelled.\n"); return; }

    /* Shift all records after this one */
    for (int i = idx; i < studentCount - 1; i++)
        students[i] = students[i + 1];

    studentCount--;
    saveToFile();
    printf("  [✓] Student record deleted.\n");
}

/* ===================== MAIN ===================== */
int main() {
    int choice;

    loadFromFile();

    printf("\n");
    printf("  *********************************************\n");
    printf("  *       FEE MANAGEMENT SYSTEM  v1.0        *\n");
    printf("  *     Track Paid / Unpaid Fees per Student  *\n");
    printf("  *********************************************\n");

    while (1) {
        printf("\n  ========== MAIN MENU ==========\n");
        printf("  1. Add New Student\n");
        printf("  2. Record Fee Payment\n");
        printf("  3. View All Students\n");
        printf("  4. Search Student by ID\n");
        printf("  5. View Pending Fees Only\n");
        printf("  6. View Fully Paid Students\n");
        printf("  7. Fee Collection Summary\n");
        printf("  8. Delete Student Record\n");
        printf("  9. Exit\n");
        printf("  ================================\n");
        printf("  Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addStudent();    break;
            case 2: payFee();        break;
            case 3: viewAll();       break;
            case 4: searchStudent(); break;
            case 5: viewPendingOnly(); break;
            case 6: viewPaidOnly();  break;
            case 7: feeSummary();    break;
            case 8: deleteStudent(); break;
            case 9:
                printf("\n  Data saved. Goodbye!\n\n");
                exit(0);
            default:
                printf("  [!] Invalid choice! Enter 1-9.\n");
        }
    }

    return 0;
}
